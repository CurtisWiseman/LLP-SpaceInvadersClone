#pragma once
#include "Actor.h"


class Bullet : public Actor
{
public:
	enum BulletState
	{
		L,
		R,
		X
	};

	Bullet(GameData* _GD, Actor* _parent);
	~Bullet() = default;

	void shoot();
	BulletState getBulletState();
	ASGE::Sprite* getSpriteL();
	ASGE::Sprite* getSpriteR();
	void swapBulletState();
	void updatePosition();

private:
	std::unique_ptr<ASGE::Sprite> sprite_l = nullptr;
	std::unique_ptr<ASGE::Sprite> sprite_r = nullptr;
	Actor* parent = nullptr;
	BulletState state = BulletState::X;
};