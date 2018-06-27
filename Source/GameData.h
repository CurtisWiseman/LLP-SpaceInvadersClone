#pragma once
#include "Engine/Sprite.h"
#include <vector>
#include <memory>
#include "SoundSystem.h"

//Sound capabilities
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

enum class GameState
{
	MENU,
	GAMEPLAY,
	GAMEOVER,
	GAMEWIN,
	GAMELOSE,
	PAUSE
};


class GameData
{
public:
	enum Sounds
	{
		PLAYER_SHOOT,
		PLAYER_DEATH,
		ENEMY_DEATH,
		UFO,
		ENEMY_MOVE1,
		ENEMY_MOVE2,
		ENEMY_MOVE3,
		ENEMY_MOVE4
	};

	GameState state;

	const int use_FMOD_sound = true;
	int lives = 3;
	int score = 0;
	int prev_extra_life_score = 1000;
	int highscore = 0;
	float difficulty_divisor = 1.0f;
	long double previous_tick;
	long double current_tick;
	long double delta_time;

	std::unique_ptr<SoundSystem> sound_sys;
	SoundSystem::SoundClass sound1;
	SoundSystem::SoundClass sound2;
	SoundSystem::SoundClass sound3;
	SoundSystem::SoundClass sound4;
	SoundSystem::SoundClass sound5;
	SoundSystem::SoundClass sound6;
	SoundSystem::SoundClass sound7;
	SoundSystem::SoundClass sound8;
	std::vector<SoundSystem::SoundClass> sounds_vector;

	std::shared_ptr<ASGE::Renderer> renderer;
};