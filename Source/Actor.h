#pragma once
#include <Engine/Sprite.h>
#include "GameData.h"


namespace ASGE
{
	class Renderer;
	class Sprite;
}


class Actor
{
public:
	enum ActorType
	{
		PLAYER,
		ENEMY,
		PLAYER_BULLET,
		ENEMY_BULLET,
		BLOCKADE,
		EXPLOSION,
		UNKNOWN_ACTOR_TYPE
	};

	Actor(GameData* _GD, const char* filename, int width, int height);
	~Actor() = default;

	bool getAlive();
	int getHealth();
	ASGE::Sprite& getAvatar();
	ActorType getActorType();
	bool isCollidingWith(Actor* other);
	void loadAvatar(const char* filename, int width, int height);
	void setAlive(bool a);
	virtual void damage();
	virtual void kill();

protected:
	std::unique_ptr<ASGE::Sprite> avatar = nullptr;
	GameData* game_data;
	ActorType a_type = UNKNOWN_ACTOR_TYPE;
	bool alive = 1;
	int health = 1;
	int max_health = 1;
	float f_pos[2] = { 0.0f, 0.0f };

private:
};