#pragma once
#include "Actor.h"


class Explosion : public Actor
{
public:
	Explosion(GameData* _GD, 
			  const char* filename1, const char* filename2,
			  int width, int height, Actor* _parent);
	void tick();
	void detonate();
	void reset();

private:
	std::unique_ptr<ASGE::Sprite> sprite_1 = nullptr;
	std::unique_ptr<ASGE::Sprite> sprite_2 = nullptr;
	float tick_timer = 0.0f;
	float tick_max = 4.0f;
	Actor* parent = nullptr;
	bool second_sprite = false;
};