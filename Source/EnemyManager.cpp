#include "EnemyManager.h"
#include "Bullet.h"
#include "Explosion.h"
#include <vector>
#include <time.h>
#include <Engine/Sprite.h>


EnemyManager::EnemyManager(GameData* _GD)
{
	game_data = _GD;

	//Setup enemies
	initEnemies();

	//Check to make sure where the first and last columns containing live enemies are
	updateCols();

	//Initialise the random seed
	srand(static_cast<unsigned int>(time(NULL)));
}


void EnemyManager::tick()
{
	//Update timers
	enemy_tick_timer += 0.1f * static_cast<float>(game_data->delta_time);
	enemy_sound_timer += 0.1f * static_cast<float>(game_data->delta_time);
	bullet_tick_timer += 0.1f * static_cast<float>(game_data->delta_time);
	
	//Randomly spawn the UFO
	if (!ufo.get()->getAlive())
	{
		int r = rand() % 100000;
		if (r < ufo_probability)
		{
			ufo.get()->setAlive(true);
			ufo.get()->getAvatar().position[0] = -100;
			ufo.get()->getAvatar().position[1] = 40;

			//Play the UFO sound via FMOD
			if (game_data->use_FMOD_sound)
			{
				game_data->sound_sys->playSound(game_data->sounds_vector[GameData::UFO], true, &ufo_sound_channel);
			}
		}
	}
	//Update the UFO's position
	else
	{
		moveUFO();
		//Manually loop sound (if we are using Windows sound that is, 
		//                           FMOD loops automatically)
		if (!game_data->use_FMOD_sound)
		{
			ufo_sound_tick += static_cast<float>(game_data->delta_time);
			if (ufo_sound_tick > ufo_sound_max)
			{
				sndPlaySound(
					L"..\\..\\Resources\\Sounds\\ufo_highpitch_quiet.wav", 
					SND_FILENAME | SND_ASYNC);
				ufo_sound_tick = 0.0f;
			}	
		}
	}

	//Tick the main block of enemies
	for (int i=0; i < num_enemies; i++)
	{
		enemies[i].get()->tick();
	}

	if (getEnemiesAlive() > 0)
	{
		//If the timer is up, move the enemies and 'flip' their sprites.
		if (enemy_tick_timer > enemy_tick_max)
		{
			flipEnemies();
			moveEnemies();
			if (enemy_sound_timer > enemy_sound_max)
			{
				if (game_data->use_FMOD_sound)
				{
					switch (enemy_tick_count)
					{
						case 0:
							game_data->sound_sys->playSound(game_data->sounds_vector[GameData::ENEMY_MOVE1], false, 0);
							break;
						case 1:
							game_data->sound_sys->playSound(game_data->sounds_vector[GameData::ENEMY_MOVE2], false, 0);
							break;
						case 2:
							game_data->sound_sys->playSound(game_data->sounds_vector[GameData::ENEMY_MOVE3], false, 0);
							break;
						case 3:
							game_data->sound_sys->playSound(game_data->sounds_vector[GameData::ENEMY_MOVE4], false, 0);
							enemy_tick_count = -1;
							break;
						default:
							enemy_tick_count = -1;
							break;
					}
				}
				else
				{
					switch (enemy_tick_count)
					{
						case 0:
							sndPlaySound(L"..\\..\\Resources\\Sounds\\fastInvader1.wav", SND_FILENAME | SND_ASYNC);
							break;
						case 1:
							sndPlaySound(L"..\\..\\Resources\\Sounds\\fastInvader2.wav", SND_FILENAME | SND_ASYNC);
							break;
						case 2:
							sndPlaySound(L"..\\..\\Resources\\Sounds\\fastInvader3.wav", SND_FILENAME | SND_ASYNC);
							break;
						case 3:
							sndPlaySound(L"..\\..\\Resources\\Sounds\\fastInvader4.wav", SND_FILENAME | SND_ASYNC);
							enemy_tick_count = -1;
							break;
						default:
							enemy_tick_count = -1;
							break;
					}
				}
				enemy_tick_count++;
				enemy_sound_timer = 0.0f;
			}
			enemy_tick_timer = 0.0f;
		}
		//Have a random bottom-of-column enemy shoot at the player.
		//This is done by randomly picking a column and then going 
		//from bottom to top to look for one that is alive (i.e. can shoot)
		if (bullet_tick_timer > bullet_tick_max)
		{
			bool found_live_enemy = 0;
			while (found_live_enemy == 0)
			{
				int r = rand() % num_cols;
				if (enemies[((num_rows - 1) * num_cols) + r].get()->getAlive())
				{
					enemies[((num_rows - 1) * num_cols) + r].get()->getBullet().shoot();
					found_live_enemy = 1;
				}
				else if (enemies[((num_rows - 2) * num_cols) + r].get()->getAlive())
				{
					enemies[((num_rows - 2) * num_cols) + r].get()->getBullet().shoot();
					found_live_enemy = 1;
				}
				else if (enemies[((num_rows - 3) * num_cols) + r].get()->getAlive())
				{
					enemies[((num_rows - 3) * num_cols) + r].get()->getBullet().shoot();
					found_live_enemy = 1;
				}
				else if (enemies[((num_rows - 4) * num_cols) + r].get()->getAlive())
				{
					enemies[((num_rows - 4) * num_cols) + r].get()->getBullet().shoot();
					found_live_enemy = 1;
				}
				else if (enemies[((num_rows - 5) * num_cols) + r].get()->getAlive())
				{
					enemies[((num_rows - 5) * num_cols) + r].get()->getBullet().shoot();
					found_live_enemy = 1;
				}
				bullet_tick_timer = 0.0f;
			}
		}
	}
	//If there are no enemies left, tell the game that the player has won
	else
	{
		game_data->state = GameState::GAMEWIN;
	}
}


int EnemyManager::getEnemiesAlive()
{
	//Get the number of currently alive enemies.
	int num_alive = 0;
	for (int i=0; i < num_enemies; i++)
	{
		if (enemies[i].get()->getAlive())
		{
			num_alive++;
		}
	}
	return num_alive;
}


void EnemyManager::flipEnemies()
{
	//Toggle between the two enemy sprites for each.
	for (int i=0; i < num_enemies; i++)
	{
		if (enemies[i].get()->getAlive())
		{
			enemies[i].get()->swapState();
		}
	}
}


void EnemyManager::moveEnemies()
{
	//Depending on which direction they're heading...
	if (moving_right)
	{
		//Move them right if they're not close to the edge
		if (enemies[last_col]->getAvatar().position[0] < 1220)
		{
			for (int i=0; i < num_enemies; i++)
			{
				enemies[i]->getAvatar().position[0] += 20;
			}
		}
		//Or move them down and change direction if they are close
		else
		{
			moving_right = false;
			for (int i=0; i < num_enemies; i++)
			{
				enemies[i]->getAvatar().position[1] += 20;
			}
		}
	}
	//Likewise for leftwards movement
	else
	{
		if (enemies[first_col]->getAvatar().position[0] > 36)
		{
			for (int i=0; i < num_enemies; i++)
			{
				enemies[i]->getAvatar().position[0] -= 20;
			}
		}
		else
		{
			moving_right = true;
			for (int i=0; i < num_enemies; i++)
			{
				enemies[i]->getAvatar().position[1] += 20;
			}
		}
	}
}


void EnemyManager::moveUFO()
{
	//Move the ufo to the right at a proportional speed (relative to delta time)
	if (ufo.get()->getAlive())
	{
		ufo_tick_timer += static_cast<float>(game_data->delta_time);
		
		if (ufo_tick_timer > ufo_tick_max)
		{
			ufo.get()->getAvatar().position[0] += static_cast<int>(ufo_speed*ufo_tick_timer);
			ufo_tick_timer -= ufo_tick_max;
		}

		//Stop its function if it has left the screen, but do not kill it
		if (ufo.get()->getAvatar().position[0] > 1300)
		{
			ufo.get()->getAvatar().position[0] = -100;
			ufo.get()->setAlive(false);
			ufo_tick_timer = 0.0f;
			ufo_sound_tick = 0.0f;
			if (game_data->use_FMOD_sound)
			{
				ufo_sound_channel->stop();
			}
		}
	}
	else
	{
		//Just to make sure it has stopped if moveUFO()
		//is ever mistakenly called when it is not alive
		ufo_tick_timer = 0.0f;
		ufo_sound_tick = 0.0f;
		if (game_data->use_FMOD_sound)
		{
			ufo_sound_channel->stop();
		}
	}
}


void EnemyManager::renderEnemies()
{
	//Render all the enemies, their explosions, and their bullets.
	if (ufo.get()->getAlive())
	{
		ufo.get()->getAvatar().render(game_data->renderer);
	}
	for (int i = 0; i < num_enemies; i++)
	{
		if (enemies[i].get()->getAlive())
		{
			enemies[i].get()->getAvatar().render(game_data->renderer);
		}
		if (enemies[i].get()->getBullet().getAlive())
		{
			enemies[i].get()->getBullet().getAvatar().render(game_data->renderer);
		}
		if (enemies[i].get()->getExplosion().getAlive())
		{
			enemies[i].get()->getExplosion().getAvatar().render(game_data->renderer);
		}
	}
}


void EnemyManager::updateSpeed()
{
	//Update the speed based on how many enemies have been killed.
	//Gets exponentially faster due to the maths involved.
	float alive = static_cast<float>(getEnemiesAlive());
	float total = static_cast<float>(num_enemies);
	enemy_tick_max = (6.0f * (alive/total)) / game_data->difficulty_divisor;
}


void EnemyManager::checkCollisionsWith(Actor* other)
{
	//Check the other object against every enemy
	//and damage the appropriate participants.
	if (other->getAlive())
	{
		for (int i=0; i < num_enemies; i++)
		{
			if (enemies[i].get()->getAlive())
			{
				if (enemies[i].get()->isCollidingWith(other))
				{
					other->damage();
					switch (other->getActorType())
					{
						case Actor::ActorType::PLAYER_BULLET:
							enemies[i].get()->damage();
							
							
							if (i > num_cols * 3)
							{
								game_data->score += 10;
							}
							else if (i > num_cols * 1)
							{
								game_data->score += 20;
							}
							else
							{
								game_data->score += 30;
							}
							break;
						case Actor::ActorType::PLAYER:
							game_data->state = GameState::GAMELOSE;
							break;
						case Actor::ActorType::BLOCKADE:
							break;
					}
					updateSpeed();
					updateCols();
				}
			}
		}
		if (ufo.get()->getAlive())
		{
			if (ufo.get()->isCollidingWith(other))
			{
				ufo.get()->damage();
				other->damage();
				int r = rand() % 3;
				switch (r)
				{
					case 0:
						game_data->score += 50;
						break;
					case 1:
						game_data->score += 100;
						break;
					case 2:
						game_data->score += 150;
						break;
				}

				ufo_tick_timer = 0.0f;
				ufo_sound_tick = 0.0f;
				if (game_data->use_FMOD_sound)
				{
					ufo_sound_channel->stop();
				}
			}
		}
	}
}


void EnemyManager::checkBulletCollisionsWith(Actor* other)
{
	//Check the other actor against every enemy's bullet.
	if (other->getAlive())
	{
		for (int i=0; i < num_enemies; i++)
		{
			if (enemies[i].get()->getBullet().getAlive())
			{
				if (enemies[i].get()->getBullet().isCollidingWith(other))
				{
					enemies[i]->getBullet().damage();
					other->damage();
				}
			}
		}
	}
}


void EnemyManager::increaseDifficulty()
{
	//Increase the base speed of the aliens as a difficulty measure.
	game_data->difficulty_divisor = game_data->difficulty_divisor + 1.0f;
}


FMOD::Channel* EnemyManager::get_ufo_sound_channel()
{
	//Return a pointer to the UFO's sound channel so the looping can be dealt with properly.
	return ufo_sound_channel;
}


void EnemyManager::updateCols()
{
	//Update the firstcol and lastcol flags,
	//so the manager knows when to make the enemies change direction.
	first_col = -1;
	for (int i=0; i < num_cols; i++)
	{
		for (int j=0; j < num_rows; j++)
		{
			if (enemies[i + (j*num_cols)]->getAlive())
			{
				first_col = i;
				break;
			}
		}
		if (first_col != -1)
		{
			break;
		}
	}
	if (first_col == -1)
	{
		first_col = 0;
	}

	last_col = 0;
	for (int i=0; i < num_cols; i++)
	{
		for (int j=0; j < num_rows; j++)
		{
			if (enemies[i + (j*num_cols)]->getAlive())
			{
				last_col = i;
			}
		}
	}
}


void EnemyManager::initEnemies()
{
	//Setup UFO
	ufo = std::make_unique<Enemy>(game_data,
								  "..\\..\\Resources\\Textures\\AlienUFO.png",
								  "..\\..\\Resources\\Textures\\AlienUFO.png",
								  64, 28);
	ufo.get()->setAlive(false);
	ufo.get()->getAvatar().position[0] = -100;
	ufo.get()->getAvatar().position[1] = 20;

	//Setup main block
	enemies.reserve(60);
	int x_pos = 80;
	int y_pos = 100;
	for (int i=0; i < num_cols; i++)
	{
		std::unique_ptr<Enemy> temp;
		temp = std::make_unique<Enemy>(game_data,
									   "..\\..\\Resources\\Textures\\AlienA1.png",
									   "..\\..\\Resources\\Textures\\AlienA2.png",
									   24, 24);
		enemies.push_back(std::move(temp));
		enemies[i].get()->getAvatar().position[0] = x_pos + 8;
		enemies[i].get()->getAvatar().position[1] = y_pos;

		x_pos += 80;
	}

	x_pos = 80;
	y_pos = 180;
	for (int i=0; i < num_cols; i++)
	{
		std::unique_ptr<Enemy> temp;
		temp = std::make_unique<Enemy>(game_data,
									   "..\\..\\Resources\\Textures\\AlienB1.png",
									   "..\\..\\Resources\\Textures\\AlienB2.png",
									   33, 24);
		enemies.push_back(std::move(temp));
		enemies[i + (num_cols)].get()->getAvatar().position[0] = x_pos + 2;
		enemies[i + num_cols].get()->getAvatar().position[1] = y_pos;

		x_pos += 80;
	}

	x_pos = 80;
	y_pos = 260;
	for (int i=0; i < num_cols; i++)
	{
		std::unique_ptr<Enemy> temp;
		temp = std::make_unique<Enemy>(game_data,
									   "..\\..\\Resources\\Textures\\AlienB1.png",
									   "..\\..\\Resources\\Textures\\AlienB2.png",
									   33, 24);
		enemies.push_back(std::move(temp));
		enemies[i + num_cols * 2].get()->getAvatar().position[0] = x_pos + 2;
		enemies[i + num_cols * 2].get()->getAvatar().position[1] = y_pos;

		x_pos += 80;
	}

	x_pos = 80;
	y_pos = 320;
	for (int i=0; i < num_cols; i++)
	{
		std::unique_ptr<Enemy> temp;
		temp = std::make_unique<Enemy>(game_data,
									   "..\\..\\Resources\\Textures\\AlienC1.png",
									   "..\\..\\Resources\\Textures\\AlienC2.png",
									   36, 24);
		enemies.push_back(std::move(temp));
		enemies[i + num_cols * 3].get()->getAvatar().position[0] = x_pos;
		enemies[i + num_cols * 3].get()->getAvatar().position[1] = y_pos;

		x_pos += 80;
	}

	x_pos = 80;
	y_pos = 400;
	for (int i=0; i < num_cols; i++)
	{
		std::unique_ptr<Enemy> temp;
		temp = std::make_unique<Enemy>(game_data,
									   "..\\..\\Resources\\Textures\\AlienC1.png",
									   "..\\..\\Resources\\Textures\\AlienC2.png",
									   36, 24);
		enemies.push_back(std::move(temp));
		enemies[i + num_cols * 4].get()->getAvatar().position[0] = x_pos;
		enemies[i + num_cols * 4].get()->getAvatar().position[1] = y_pos;

		x_pos += 80;
	}
}