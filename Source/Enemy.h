#pragma once
#include "Actor.h"


class Bullet;
class Explosion;


class Enemy : public Actor
{
public:
	Enemy(GameData* _GD,
		  const char* file1, const char* file2,
		  int width, int height);

	~Enemy() = default;

	enum EnemyState
	{
		L,
		R,
		X
	};

	Bullet& getBullet();
	Explosion& getExplosion();
	EnemyState getState();
	void swapState();
	void tick();
	void kill();
	
private:
	float bullet_flip_timer = 0.0f;
	const float bullet_flip_max = 1.0f;
	std::unique_ptr<Bullet> e_bull = nullptr;
	std::unique_ptr<ASGE::Sprite> sprite_l = nullptr;
	std::unique_ptr<ASGE::Sprite> sprite_r = nullptr;
	std::unique_ptr<Explosion> e_explode = nullptr;
	EnemyState state = EnemyState::X;
};