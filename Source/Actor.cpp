#include "Actor.h"
#include <Engine/Renderer.h>


Actor::Actor(GameData* _GD, const char* filename, int width, int height)
{
	//Setup variables
	game_data = _GD;
	loadAvatar(filename, width, height);
	setAlive(true);
}


bool Actor::getAlive()
{
	//Returns a value to say whether this actor is alive or not.
	return alive;
}


int Actor::getHealth()
{
	//Returns the currently remaining health of the actor.
	return health;
}


ASGE::Sprite& Actor::getAvatar()
{
	//Returns a pointer to the avatar to be manipulated.
	return *avatar;
}


Actor::ActorType Actor::getActorType()
{
	//An enum for other objects to easily identify this actor
	return a_type;
}


bool Actor::isCollidingWith(Actor* other)
{
	//if this left edge < other right edge && this right edge > other left edge
	//ditto for top/bottom
	if (this->getAvatar().position[0] < (other->getAvatar().position[0] + other->getAvatar().size[0]) &&
		(this->getAvatar().position[0] + this->getAvatar().size[0]) > other->getAvatar().position[0])
	{
		if (this->getAvatar().position[1] < (other->getAvatar().position[1] + other->getAvatar().size[1]) &&
			(this->getAvatar().position[1] + this->getAvatar().size[1]) > other->getAvatar().position[1])
		{
			return true;
		}
	}
	return false;
}


void Actor::loadAvatar(const char* filename, int width, int height)
{
	//Load in a sprite with the given filename
	avatar = game_data->renderer->createSprite();
	avatar->loadTexture(filename);
	avatar->size[0] = width;
	avatar->size[1] = height;
}


void Actor::setAlive(bool a)
{
	//Sets the 'alive' state of the actor and its health accordingly.
	if (a == true)
	{
		health = max_health;
	}
	else
	{
		health = 0;
	}
	alive = a;
}


void Actor::damage()
{
	//Removes health from the actor, killing them if their health drops below 1
	health--;
	if (health <= 0)
	{
		kill();
	}
}


void Actor::kill()
{
	//Goes through any special behaviour for when the actor is 'killed'.
	//Can be overridden.
	setAlive(false);
}