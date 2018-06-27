#include "Game.h"
#include "Actions.h"
#include "Constants.h"
#include "GameFont.h"
#include <Engine/Input.h>
#include <Engine/Keys.h>
#include <Engine/Sprite.h>

#include "GameData.h"
#include "Player.h"
#include "Bullet.h"
#include "EnemyManager.h"
#include <fmod.hpp>
#include <time.h>
#include <chrono>


InvadersGame::InvadersGame()
{

}


InvadersGame::~InvadersGame()
{
	this->inputs->unregisterCallback(callback_id);
	for (int i = 0; i < 8; i++)
	{
		game_data->sound_sys->SoundSystem::releaseSound(game_data->sounds_vector[i]);
	}
}


bool InvadersGame::init()
{
	//Set up everything needed to run
	game_data = std::make_unique<GameData>();
	game_data.get()->state = GameState::MENU;			/**< Set the initial gamestate. */

	initSounds();
	
	width = WINDOW_WIDTH;
	height = WINDOW_HEIGHT;
	if (!initAPI())
	{
		return false;
	}
	game_data->renderer = renderer;
	renderer->setWindowTitle("Invaders - Exercise 1");
	renderer->setClearColour(ASGE::COLOURS::BLACK);
	//toggleFPS();

	// input callback function
	callback_id = this->inputs->addCallbackFnc(&InvadersGame::input, this);
	
	// Try a few different fonts out
	GameFont::fonts[0] = new GameFont(
		renderer->loadFont("..\\..\\Resources\\Fonts\\Comic.ttf", 42), "default", 42);
	GameFont::fonts[1] = new GameFont(
		renderer->loadFont("..\\..\\Resources\\Fonts\\DroidSansMono.ttf", 42), "default", 42);
	GameFont::fonts[2] = new GameFont(
		renderer->loadFont("..\\..\\Resources\\Fonts\\Headline.ttf", 42), "default", 42);
	GameFont::fonts[3] = new GameFont(
		renderer->loadFont("..\\..\\Resources\\Fonts\\Monkey.ttf", 42), "default", 42);
	
	if (GameFont::fonts[0]->id == -1 || GameFont::fonts[1]->id == -1 || GameFont::fonts[2]->id == -1 || GameFont::fonts[3]->id == -1)
	{
		return false;
	}

	//Use this font - consistent styling
	renderer->setFont(GameFont::fonts[2]->id);

	//Load the title screen
	sprite = renderer->createSprite();
	sprite->position[0] = 0;
	sprite->position[1] = 0;

	if (!sprite->loadTexture("..\\..\\Resources\\Textures\\TitleScreen.png"))
	{
		return false;
	}

	//Initialise the other states, 
	//ready to switch to at a moment's notice
	initGameplay();
	initPause();
	initGameover();
	return true;
}


bool InvadersGame::initGameplay()
{
	// load player sprite
	player_one = std::make_unique<Player>(game_data.get(), "..\\..\\Resources\\Textures\\Player.png", 52, 32);

	player_one.get()->getAvatar().position[0] = 614;
	player_one.get()->getAvatar().position[1] = 650;

	// spawn in the enemy manager

	enemy_man = std::make_unique<EnemyManager>(game_data.get());

	//initialise the blockades
	initBlockades();

	return true;
}


bool InvadersGame::initPause()
{
	// load pause screen
	pause_screen = renderer->createSprite();
	pause_screen->position[0] = 0;
	pause_screen->position[1] = 0;

	if (!pause_screen->loadTexture("..\\..\\Resources\\Textures\\PauseScreen.png"))
	{
		return false;
	}
	return true;
}


bool InvadersGame::initGameover()
{
	// load gameover screen
	gameover_screen = renderer->createSprite();
	gameover_screen->position[0] = 0;
	gameover_screen->position[1] = 0;

	if (!gameover_screen->loadTexture("..\\..\\Resources\\Textures\\GameOverScreen.png"))
	{
		return false;
	}
	return true;
}


bool InvadersGame::run()
{
	while (!shouldExit())
	{
		//Process all game actions, should be done on every state
		processGameActions();

		//Update delta time in milliseconds
		std::chrono::milliseconds ms = 
			std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch());
		game_data->previous_tick = game_data->current_tick;
		game_data->current_tick = static_cast<long double>(ms.count());
		game_data->delta_time = (game_data->current_tick - game_data->previous_tick) / 16;

		//Gameplay-specific updates
		if (game_data->state == GameState::GAMEPLAY)
		{
			checkCollisions();
			if (game_data->score > game_data->highscore)
			{
				game_data->highscore = game_data->score;
			}
			if (game_data->score >= (game_data->prev_extra_life_score))
			{
				player_one->extraLife();
				game_data->prev_extra_life_score += 1000;
			}
		}
		//Reset the level without wiping progress
		else if (game_data->state == GameState::GAMEWIN)
		{
			resetLevel();
			enemy_man.get()->increaseDifficulty();
			game_data->state = GameState::GAMEPLAY;
		}
		//Reset, wipe progress (retain high score) and send to game over
		else if (game_data->state == GameState::GAMELOSE)
		{
			resetLevel();
			game_data->score = 0;
			game_data->lives = 3;
			game_data->prev_extra_life_score = 1000;
			game_data->difficulty_divisor = 1.0f;
			game_data->state = GameState::GAMEOVER;
		}

		//Update the visuals and sound system
		render();
		game_data->sound_sys->m_pSystem->update();
	}
	return true;
}


bool InvadersGame::shouldExit() const
{
	//Return a value if either the renderer ot game has decided to exit
	return (renderer->exit() || this->exit);
}


void InvadersGame::render()
{
	//Render all the frames that need to be drawn.
	//Gameplay is drawn under the transluscent pause screen,
	//so you can see it paused.
	beginFrame();
	switch (game_data->state)
	{
		case GameState::MENU:
			drawFrame();
			break;
		case GameState::GAMEPLAY:
			drawFrameGame();
			break;
		case GameState::PAUSE:
			drawFrameGame();
			drawFramePause();
			break;
		case GameState::GAMEOVER:
			drawFrameGameover();
			break;
	}
	endFrame();
}


void InvadersGame::drawFrame()
{
	//Title screen.
	sprite->render(renderer);
}


void InvadersGame::drawFrameGame()
{
	//Draw all the game objects
	if (player_one->getAlive())
	{
		player_one.get()->getAvatar().render(renderer);
	}
	if (player_one->getBullet().getAlive())
	{
		player_one->getBullet().getAvatar().render(renderer);
	}
	if (player_one->getExplosion().getAlive())
	{
		player_one->getExplosion().getAvatar().render(renderer);
	}
	enemy_man->renderEnemies();

	for (int i=0; i < num_blockades * 6; i++)
	{
		if (blockades[i].get()->getAlive())
		{
			blockades[i].get()->getAvatar().render(renderer);
		}
	}

	std::string str_score = "Score: " + std::to_string(game_data->score);
	const char* chr_score = str_score.c_str();
	renderer->renderText(chr_score, 20, 30, 0.75, ASGE::COLOURS::WHITE);
	
	//Some debugging values
	/*
	std::string str_lscore = "Score til next life: " + std::to_string(game_data->prev_extra_life_score);
	const char* chr_lscore = str_lscore.c_str();
	renderer->renderText(chr_lscore, 20, 60, 0.5, ASGE::COLOURS::WHITE);
	
	std::string str_lvscore = "Lives: " + std::to_string(game_data->lives);
	const char* chr_lvscore = str_lvscore.c_str();
	renderer->renderText(chr_lvscore, 20, 100, 0.5, ASGE::COLOURS::WHITE);
	*/

	std::string str_hscore = "Highscore: " + std::to_string(game_data->highscore);
	const char* chr_hscore = str_hscore.c_str();
	renderer->renderText(chr_hscore, 575, 30, 0.75, ASGE::COLOURS::WHITE);

	std::string str_difficulty = "Difficulty: " + std::to_string(static_cast<int>(game_data->difficulty_divisor));
	const char* chr_difficulty = str_difficulty.c_str();
	renderer->renderText(chr_difficulty, 1135, 30, 0.75, ASGE::COLOURS::WHITE);

	int p_x = player_one.get()->getAvatar().position[0];
	int p_y = player_one.get()->getAvatar().position[1];
	for (int i = 0; i < player_one.get()->getLives(); i++)
	{
		//render extra life graphic
		player_one.get()->getAvatar().position[0] = (20 + (60 * i));
		player_one.get()->getAvatar().position[1] = (680);
		player_one.get()->getAvatar().render(renderer);
	}
	player_one.get()->getAvatar().position[0] = p_x;
	player_one.get()->getAvatar().position[1] = p_y;
}


void InvadersGame::drawFramePause()
{
	//Draw pause screen
	pause_screen->render(renderer);
}


void InvadersGame::drawFrameGameover()
{
	//Draw game over screen
	gameover_screen->render(renderer);
}


void InvadersGame::resetLevel()
{
	//Stop any repeating sounds
	enemy_man->get_ufo_sound_channel()->stop();

	//Freeing memory of current state
	enemy_man.release();
	player_one.release();
	blockades.clear();

	//Reinitialisation
	initGameplay();
}


void InvadersGame::input(int key, int action) const
{
	//Process keyboard inputs
	if (action == ASGE::KEYS::KEY_PRESSED)
	{
		if (key == ASGE::KEYS::KEY_ESCAPE)
		{
			game_action = GameAction::EXIT;
		}
	}
	switch (game_data->state)
	{
		case GameState::MENU:
			if (action == ASGE::KEYS::KEY_PRESSED)
			{
				if (key == ASGE::KEYS::KEY_W)
				{
					game_action = GameAction::MENUUP;
				}
				if (key == ASGE::KEYS::KEY_S)
				{
					game_action = GameAction::MENUDOWN;
				}
				if (key == ASGE::KEYS::KEY_ENTER || key == ASGE::KEYS::KEY_SPACE)
				{
					game_action = GameAction::MENUSELECT;
				}
			}
			break;
		case GameState::GAMEPLAY:
			if (action == ASGE::KEYS::KEY_PRESSED)
			{
				if (key == ASGE::KEYS::KEY_W || key == ASGE::KEYS::KEY_SPACE)
				{
					game_action = GameAction::SHOOT;
				}
				if (key == ASGE::KEYS::KEY_ENTER)
				{
					game_action = GameAction::PAUSE;
				}
				if (key == ASGE::KEYS::KEY_D)
				{
					player_one->setState(Player::MovementState::R);
				}
				else if (key == ASGE::KEYS::KEY_A)
				{
					player_one->setState(Player::MovementState::L);
				}
			}
			else if (action == ASGE::KEYS::KEY_RELEASED)
			{
				if (key == ASGE::KEYS::KEY_D && player_one->getState() == Player::MovementState::R || key == ASGE::KEYS::KEY_A && player_one->getState() == Player::MovementState::L)
				{
					player_one->setState(Player::MovementState::N);
				}
				
			}
			break;
		case GameState::PAUSE:
			if (action == ASGE::KEYS::KEY_PRESSED)
			{
				if (key == ASGE::KEYS::KEY_ENTER)
				{
					game_action = GameAction::PAUSE;
				}
			}
			break;
		case GameState::GAMEOVER:
			if (action == ASGE::KEYS::KEY_PRESSED)
			{
				game_data->state = GameState::MENU;
			}
			break;
	}
}


void InvadersGame::processGameActions()
{
	//Handle actions based on state,
	//for instance the Enter button is either start, pause or unpause
	if (game_action == GameAction::EXIT)
	{
		this->exit = true;
	}

	bool paused = false;
	switch (game_data->state)
	{
		case GameState::PAUSE:
			if (game_action == GameAction::PAUSE)
			{
				enemy_man->get_ufo_sound_channel()->getPaused(&paused);
				if(paused)
				{
					enemy_man->get_ufo_sound_channel()->setPaused(false);
				}
				game_data->state = GameState::GAMEPLAY;
			}
			break;


		case GameState::GAMEPLAY:
			switch (game_action)
			{
				case GameAction::PAUSE:
					enemy_man->get_ufo_sound_channel()->getPaused(&paused);
					if (!paused)
					{
						enemy_man->get_ufo_sound_channel()->setPaused(true);
					}
					game_data->state = GameState::PAUSE;
					break;
				case GameAction::SHOOT:
					if (player_one.get()->getAlive())
					{
						player_one->getBullet().shoot();
					}
					break;
			}
			
			//Tick the player and enemy manager
			player_one->tick();
			enemy_man->tick();
			break;


		case GameState::MENU:
			if (game_action == GameAction::MENUSELECT)
			{
				game_data->state = GameState::GAMEPLAY;
			}
			break;
	}

	game_action = GameAction::NONE;
}


void InvadersGame::checkCollisions()
{
	//Is enemy colliding with anything?
	enemy_man.get()->checkCollisionsWith(player_one.get());
	enemy_man.get()->checkCollisionsWith(&player_one.get()->getBullet());
	for (int i = 0; i < num_blockades * 6; i++)
	{
		enemy_man.get()->checkCollisionsWith(blockades[i].get());
	}

	//Is enemy bullet colliding with anything?
	enemy_man.get()->checkBulletCollisionsWith(player_one.get());
	enemy_man.get()->checkBulletCollisionsWith(&player_one.get()->getBullet());
	for (int i = 0; i < num_blockades * 6; i++)
	{
		enemy_man.get()->checkBulletCollisionsWith(blockades[i].get());
	}

	//Is player bullet hitting blockade?
	for (int i = 0; i < num_blockades * 6; i++)
	{
		if (blockades[i].get()->isCollidingWith(&player_one.get()->getBullet()) && blockades[i].get()->getAlive() && player_one.get()->getBullet().getAlive())
		{
			player_one.get()->getBullet().damage();
			blockades[i].get()->damage();
		}
	}
}


void InvadersGame::initBlockades()
{
	//Set up all the blockades
	for (int i=0; i < num_blockades; i++)
	{
		for (int j=0; j < 6; j++)
		{
			std::string filename1 = "..\\..\\Resources\\Textures\\Blockade" + std::to_string(j) + "-1.png";
			std::string filename2 = "..\\..\\Resources\\Textures\\Blockade" + std::to_string(j) + "-2.png";
			std::string filename3 = "..\\..\\Resources\\Textures\\Blockade" + std::to_string(j) + "-3.png";

			std::unique_ptr<Blockade> temp;
			temp = std::make_unique<Blockade>(game_data.get(), filename1.c_str(), filename2.c_str(), filename3.c_str(), 32, 32);
			blockades.push_back(std::move(temp));

			int x_offset = j % 3;
			int y_offset = j / 3;

			blockades[(i*6) + j ].get()->getAvatar().position[0] = 192 + (i * 200)+ (x_offset * 32);
			blockades[(i*6) + j ].get()->getAvatar().position[1] = 550 + (y_offset * 32);
		}
	}
}


void InvadersGame::initSounds()
{
	//Initialise the sound system
	game_data->sound_sys = std::make_unique<SoundSystem>();
	
	game_data->sounds_vector.push_back((game_data->sound1));
	game_data->sounds_vector.push_back((game_data->sound2));
	game_data->sounds_vector.push_back((game_data->sound3));
	game_data->sounds_vector.push_back((game_data->sound4));
	game_data->sounds_vector.push_back((game_data->sound5));
	game_data->sounds_vector.push_back((game_data->sound6));
	game_data->sounds_vector.push_back((game_data->sound7));
	game_data->sounds_vector.push_back((game_data->sound8));

	game_data->sound_sys
		->createSound(&game_data->sounds_vector[GameData::ENEMY_DEATH],
					  "..\\..\\Resources\\Sounds\\invaderkilled.wav");
	game_data->sound_sys
		->createSound(&game_data->sounds_vector[GameData::ENEMY_MOVE1],
					  "..\\..\\Resources\\Sounds\\fastInvader1.wav");
	game_data->sound_sys
		->createSound(&game_data->sounds_vector[GameData::ENEMY_MOVE2],
					  "..\\..\\Resources\\Sounds\\fastInvader2.wav");
	game_data->sound_sys
		->createSound(&game_data->sounds_vector[GameData::ENEMY_MOVE3],
					  "..\\..\\Resources\\Sounds\\fastInvader3.wav");
	game_data->sound_sys
		->createSound(&game_data->sounds_vector[GameData::ENEMY_MOVE4],
					  "..\\..\\Resources\\Sounds\\fastInvader4.wav");
	game_data->sound_sys
		->createSound(&game_data->sounds_vector[GameData::PLAYER_SHOOT],
					  "..\\..\\Resources\\Sounds\\shoot.wav");
	game_data->sound_sys
		->createSound(&game_data->sounds_vector[GameData::PLAYER_DEATH],
					  "..\\..\\Resources\\Sounds\\explosion.wav");
	game_data->sound_sys
		->createSound(&game_data->sounds_vector[GameData::UFO],
					  "..\\..\\Resources\\Sounds\\ufo_highpitch_quiet.wav");
}