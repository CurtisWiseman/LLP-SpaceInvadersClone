#include "Enemy.h"
#include "Bullet.h"
#include "Explosion.h"
#include <Engine/Renderer.h>


Enemy::Enemy(GameData* _GD,
			 const char* file1, const char* file2, 
			 int width, int height) 
	: Actor (_GD, file1, width, height)
{
	//Setup variables
	a_type = ENEMY;

	game_data = _GD;

	//Load in the two sprites
	sprite_l = game_data->renderer->createSprite();
	sprite_l->loadTexture(file1);

	sprite_r = game_data->renderer->createSprite();
	sprite_r->loadTexture(file2);

	avatar = std::move(sprite_l);
	state = Enemy::EnemyState::L;

	//Make the bullet
	e_bull = std::make_unique<Bullet>(game_data, this);

	//Make the xplosion for when this alien dies
	e_explode = 
		std::make_unique<Explosion>
		(game_data, 
		 "..\\..\\Resources\\Textures\\pop1.png", 
		 "..\\..\\Resources\\Textures\\pop2.png", 
		 52, 32, this);
	
	this->setAlive(true);
}


Bullet& Enemy::getBullet()
{
	//Returns a pointer to the bullet, useful for collisions.
	return *e_bull;
}


Explosion& Enemy::getExplosion()
{
	//Returns a pointer to the explosion, for remote detonation purposes.
	return *e_explode;
}


Enemy::EnemyState Enemy::getState()
{
	//Returns the current state of the enemy,
	//moving left or moving right.
	return state;
}


void Enemy::swapState()
{
	//Swaps between rightwards movement and leftwards.
	if (state == Enemy::EnemyState::L)
	{
		sprite_l = std::move(avatar);
		avatar = std::move(sprite_r);
		avatar->position[0] = sprite_l->position[0];
		avatar->position[1] = sprite_l->position[1];
		state = Enemy::EnemyState::R;
	}
	else if (state == Enemy::EnemyState::R)
	{
		sprite_r = std::move(avatar);
		avatar = std::move(sprite_l);
		avatar->position[0] = sprite_r->position[0];
		avatar->position[1] = sprite_r->position[1];
		state = Enemy::EnemyState::L;
	}
}


void Enemy::tick()
{
	//Flip the bullet if it is time to. Also update its position.
	if (e_bull->getAlive())
	{
		bullet_flip_timer += static_cast<float>(0.1f * game_data->delta_time);
		e_bull->updatePosition();
		if (bullet_flip_timer > bullet_flip_max)
		{
			e_bull->swapBulletState();
			bullet_flip_timer = 0.0f;
		}
	}

	//Tick the explosion even if this enemy is not alive.
	e_explode->tick();
}


void Enemy::kill()
{
	//Detonate the explosion and play the enemy killed sound, 
	//before killing the enemy as normal.
	e_explode->detonate();
	if (game_data->use_FMOD_sound)
	{
		game_data->sound_sys->playSound(
			game_data->sounds_vector[GameData::ENEMY_DEATH], 
			false, 0);
	}
	else
	{
		sndPlaySound(L"..\\..\\Resources\\Sounds\\invaderkilled.wav", 
					 SND_FILENAME | SND_ASYNC);
	}
	Actor::kill();
}