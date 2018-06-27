#include "Player.h"
#include "Bullet.h"
#include "Explosion.h"

//Sound capabilities
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")


Player::Player(GameData* _GD, const char* filename, 
			   int width, int height) 
	: Actor(_GD, filename, width, height)
{
	//Set up variables
	a_type = PLAYER;

	game_data = _GD;

	//Create bullet
	p_bull = std::make_unique<Bullet>(game_data, this);

	//Create explosion for player death
	p_explode = 
		std::make_unique<Explosion>
		(game_data, 
		 "..\\..\\Resources\\Textures\\Explosion1.png", 
		 "..\\..\\Resources\\Textures\\Explosion2.png", 
		 52, 32, this);

	this->setAlive(true);
}


Bullet& Player::getBullet()
{
	//Return a pointer to the player's bullet.
	return *p_bull;
}


Explosion& Player::getExplosion()
{
	//Returns a pointer to the explosion for remote detonation.
	return *p_explode;
}


void Player::tick()
{
	//Respawn after a short time if dead.
	if (!this->getAlive())
	{
		this->setState(MovementState::N);
		respawn_timer += static_cast<float>(0.1f * game_data->delta_time);
		if (respawn_timer > respawn_max && game_data->lives > 0)
		{
			respawn_timer = 0.0f;
			this->getAvatar().position[0] = 614;
			this->getExplosion().reset();
			this->setAlive(true);
		}
	}

	//Update bullet's position.
	if (this->getBullet().getAlive())
	{
		if (this->getBullet().getAvatar().position[1] < 1)
		{
			this->getBullet().setAlive(false);
			this->getBullet().getAvatar().position[1] = 600;
		}
		this->getBullet().updatePosition();
	}

	//Update player position based on movement state and delta time.
	f_pos[0] += 4.0f * static_cast<float>(game_data->delta_time);
	if (f_pos[0] > 1.0f)
	{
		switch (state)
		{
			case MovementState::R:
				this->getAvatar().position[0] += static_cast<int>(f_pos[0]);
				f_pos[0] -= static_cast<int>(f_pos[0]);
				break;
			case MovementState::L:
				this->getAvatar().position[0] -= static_cast<int>(f_pos[0]);
				f_pos[0] -= static_cast<int>(f_pos[0]);
				break;
			case MovementState::N:
				f_pos[0] -= static_cast<int>(f_pos[0]);
				break;
		}
	}

	//Stop the player from leaving the edges of the screen.
	if (this->getAvatar().position[0] < 0)
	{
		this->getAvatar().position[0] = 0;
	}
	if (this->getAvatar().position[0] > 1228)
	{
		this->getAvatar().position[0] = 1228;
	}

	//Tick the explosion even if the player is dead.
	p_explode->tick();
}


void Player::setState(MovementState _state)
{
	//Set the player's movement state, 
	//used for smoother movement than if frame-by-frame movement was used.
	state = _state;
}


Player::MovementState Player::getState()
{
	//Get the player's movement state for input handling.
	return state;
}


int Player::getLives()
{
	//Get the number of lives.
	return game_data->lives;
}


void Player::extraLife()
{
	//Add an extra life (up to 5 max)
	if (game_data->lives < 5)
	{
		game_data->lives++;
	}
}


void Player::kill()
{
	//Remove a life, check for game over,
	//and detonate the explosion before calling Actor::kill().
	game_data->lives--;
	if (game_data->lives == 0)
	{
		game_data->state = GameState::GAMELOSE;
	}

	if (game_data->use_FMOD_sound)
	{
		game_data->sound_sys->playSound(game_data->sounds_vector[1], false, 0);
	}
	else
	{
		sndPlaySound(L"..\\..\\Resources\\Sounds\\explosion.wav", SND_FILENAME | SND_ASYNC);
	}
	
	p_explode->detonate();

	Actor::kill();
}