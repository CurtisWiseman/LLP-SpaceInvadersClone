#pragma once
#include "Actor.h"


class Blockade : public Actor
{
public:
	Blockade(GameData* _GD, const char* filename1, const char* filename2, const char* filename3, int width, int height);
	void damage() override;

private:
	std::unique_ptr<ASGE::Sprite> sprite_1 = nullptr;
	std::unique_ptr<ASGE::Sprite> sprite_2 = nullptr;
	std::unique_ptr<ASGE::Sprite> sprite_3 = nullptr;
};