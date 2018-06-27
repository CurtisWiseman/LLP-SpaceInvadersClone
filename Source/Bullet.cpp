#include "Bullet.h"
#include <Engine/Renderer.h>

//Sound capabilities
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")


Bullet::Bullet(GameData* _GD, Actor* _parent)
	: Actor(_GD, "..\\..\\Resources\\Textures\\BulletP.png", 3, 12)
{
	//Setup variables
	parent = _parent;
	setAlive(false);
	game_data = _GD;

	if (parent->getActorType() == ENEMY)
	{
		//Load in 'zigzag' sprites
		sprite_l = game_data->renderer->createSprite();
		sprite_r = game_data->renderer->createSprite();

		sprite_l->loadTexture("..\\..\\Resources\\Textures\\BulletL.png");
		sprite_r->loadTexture("..\\..\\Resources\\Textures\\BulletR.png");

		avatar = std::move(sprite_l);
		state = Bullet::BulletState::L;
		a_type = ENEMY_BULLET;
	}
	else if (parent->getActorType() == PLAYER)
	{
		//Load in player bullet sprite
		state = Bullet::BulletState::X;
		a_type = PLAYER_BULLET;
	}
}


void Bullet::shoot()
{
	if (!this->getAlive())
	{
		//If this bullet belongs to an enemy,
		//randomise to pick either the more common
		//cross-shaped bullet or slightly rarer zigzag one.
		int r = rand() % 100;
		if (r > 40 && parent->getActorType() == ENEMY)
		{
			sprite_l = game_data->renderer->createSprite();
			sprite_r = game_data->renderer->createSprite();
			sprite_l->loadTexture("..\\..\\Resources\\Textures\\BulletE.png");
			sprite_r->loadTexture("..\\..\\Resources\\Textures\\BulletE.png");
			avatar = std::move(sprite_l);
			state = Bullet::BulletState::L;
		}

		//If this bullet belongs to the player, play the shoot sound.
		if (parent->getActorType() == PLAYER)
		{
			if (game_data->use_FMOD_sound)
			{
				game_data->sound_sys->playSound(game_data->sounds_vector[GameData::PLAYER_SHOOT], false, 0);
			}
			else
			{
				sndPlaySound(L"..\\..\\Resources\\Sounds\\shoot.wav", SND_FILENAME | SND_ASYNC);
			}
		}
		//Also put it centered above the player and set it to be alive.
		this->getAvatar().position[0]
			= parent->getAvatar().position[0] + 24;
		this->getAvatar().position[1]
			= parent->getAvatar().position[1];
		this->setAlive(true);
	}
}


Bullet::BulletState Bullet::getBulletState()
{
	//Returns the current state of the bullet, useful for animation
	return state;
}


ASGE::Sprite* Bullet::getSpriteL()
{
	//Get a pointer to the left sprite.
	return sprite_l.get();
}


ASGE::Sprite* Bullet::getSpriteR()
{
	//Get a pointer to the right sprite.
	return sprite_r.get();
}


void Bullet::swapBulletState()
{
	//Swap between two sprites
	if (state == Bullet::BulletState::L)
	{
		sprite_l = std::move(avatar);
		avatar = std::move(sprite_r);
		avatar->position[0] = sprite_l->position[0];
		avatar->position[1] = sprite_l->position[1];
		state = Bullet::BulletState::R;
	}
	else if (state == Bullet::BulletState::R)
	{
		sprite_r = std::move(avatar);
		avatar = std::move(sprite_l);
		avatar->position[0] = sprite_r->position[0];
		avatar->position[1] = sprite_r->position[1];
		state = Bullet::BulletState::L;
	}
}


void Bullet::updatePosition()
{
	//Update float position array
	f_pos[1] += static_cast<float>(game_data->delta_time);

	//If the amount to move is greater than one (i.e. won't be rounded to zero)...
	if (f_pos[1] > 1.0f)
	{
		//If child of enemy, then move downwards
		if (parent->getActorType() == ENEMY)
		{
			this->getAvatar().position[1] += static_cast<int>(2 * f_pos[1]);
			if (this->getAvatar().position[1] > 720)
			{
				this->setAlive(false);
			}
		}
		//If child of player, then move upwards
		else
		{
			this->getAvatar().position[1] -= static_cast<int>(6 * f_pos[1]);
			if (this->getAvatar().position[1] < 0)
			{
				this->setAlive(false);
			}
		}
		//Take away the integer value that the bullet has moved.
		f_pos[1] -= static_cast<int>(f_pos[1]);
	}
}