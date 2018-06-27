#include "Blockade.h"
#include <Engine/Renderer.h>


Blockade::Blockade(GameData* _GD, const char* filename1, const char* filename2, const char* filename3, int width, int height) : Actor (_GD, filename1, width, height)
{
	//Setup variables
	max_health = 3;
	health = 3;

	//Load in the three sprites
	sprite_1 = game_data->renderer->createSprite();
	sprite_1->loadTexture(filename1);

	sprite_2 = game_data->renderer->createSprite();
	sprite_2->loadTexture(filename2);

	sprite_3 = game_data->renderer->createSprite();
	sprite_3->loadTexture(filename3);

	avatar = std::move(sprite_3);

	//Set type
	a_type = BLOCKADE;
}


void Blockade::damage()
{
	//Cycle through the different sprites
	switch (getHealth())
	{
		case 3:
			sprite_3 = std::move(avatar);
			avatar = std::move(sprite_2);
			avatar->position[0] = sprite_3->position[0];
			avatar->position[1] = sprite_3->position[1];
			break;
		case 2:
			sprite_2 = std::move(avatar);
			avatar = std::move(sprite_1);
			avatar->position[0] = sprite_2->position[0];
			avatar->position[1] = sprite_2->position[1];
			break;
			break;
		case 1:
			sprite_1 = std::move(avatar);
			avatar = std::move(sprite_3);
			avatar->position[0] = sprite_1->position[0];
			avatar->position[1] = sprite_1->position[1];
			break;
		case 0:
			break;
	}

	//Damage
	Actor::damage();
}