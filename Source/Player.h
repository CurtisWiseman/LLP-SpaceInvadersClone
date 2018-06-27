#pragma once
#include "Actor.h"


class Bullet;
class Explosion;


class Player : public Actor
{
public:
	Player(GameData* _GD, const char* filename, 
		   int width, int height);
	~Player() = default;

	enum MovementState
	{
		R,
		L,
		N
	};
	Bullet& getBullet();
	Explosion& getExplosion();
	void tick();
	void setState(MovementState _state);
	MovementState getState();
	int getLives();
	void extraLife();
	void kill() override;

private:
	std::unique_ptr<Bullet> p_bull = nullptr;
	std::unique_ptr<Explosion> p_explode = nullptr;
	float respawn_timer = 0.0f;
	float respawn_max = 8.0f;
	MovementState state = N;
};