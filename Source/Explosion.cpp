#include "Explosion.h"
#include <Engine/Renderer.h>


Explosion::Explosion(GameData* _GD,
					 const char* filename1, const char* filename2, 
					 int width, int height, Actor* _parent)
	: Actor(_GD, filename1, width, height)
{
	//Set up variables
	parent = _parent;

	//Load in sprites
	sprite_1 = game_data->renderer->createSprite();
	sprite_1->loadTexture(filename1);

	sprite_2 = game_data->renderer->createSprite();
	sprite_2->loadTexture(filename2);

	avatar = std::move(sprite_1);

	a_type = EXPLOSION;

	this->setAlive(false);
}


void Explosion::tick()
{
	//Change sprites on timer
	if (getAlive())
	{
		tick_timer += static_cast<float>(0.1*game_data->delta_time);
		
		if (tick_timer >= tick_max)
		{
			this->kill();
		}
		else if (tick_timer >= tick_max / 2 && second_sprite == false)
		{
			sprite_1 = std::move(avatar);
			avatar = std::move(sprite_2);
			second_sprite = true;
		}
	}
}


void Explosion::detonate()
{
	//Get the parent's position and start the timer
	avatar->position[0] = parent->getAvatar().position[0];
	avatar->position[1] = parent->getAvatar().position[1];
	sprite_2->position[0] = parent->getAvatar().position[0];
	sprite_2->position[1] = parent->getAvatar().position[1];
	this->setAlive(true);
}


void Explosion::reset()
{
	//Reset back to the initialised state to be used again.
	sprite_2 = std::move(avatar);
	avatar = std::move(sprite_1);
	second_sprite = false;
	tick_timer = 0.0f;
}