#pragma once
#include <Engine/Sprite.h>
#include <vector>
#include "Enemy.h"
#include "GameData.h"
#include <fmod.hpp>


class EnemyManager
{
public:
	EnemyManager(GameData* _GD);
	~EnemyManager() = default;
	void tick();
	int getEnemiesAlive();
	void flipEnemies();
	void moveEnemies();
	void moveUFO();
	void renderEnemies();
	void updateSpeed();
	void checkCollisionsWith(Actor* other);
	void checkBulletCollisionsWith(Actor* other);
	void increaseDifficulty();
	FMOD::Channel* get_ufo_sound_channel();

private:
	void updateCols();
	void initEnemies();

	std::vector<std::unique_ptr<Enemy>> enemies;
	std::unique_ptr<Enemy> ufo;

	const int num_rows = 5;
	const int num_cols = 11;
	const int num_enemies = num_rows*num_cols;

	int first_col;
	int last_col;

	float enemy_tick_timer = 0.0f;
	float enemy_tick_max = 6.0f;
	int enemy_tick_count = 0;
	float enemy_sound_timer = 0.0f;
	const float enemy_sound_max = 0.8f;

	float ufo_tick_timer = 0.0f;
	const float ufo_tick_max = 0.5f;
	float ufo_sound_tick = 0.0f;
	const float ufo_sound_max = 10.0f;
	FMOD::Channel* ufo_sound_channel = nullptr;

	float bullet_tick_timer = 0.0f;
	const float bullet_tick_max = 4.0f;

	const float ufo_speed = 4.0f;
	const int ufo_probability = 1;

	bool moving_right = true;

	GameData* game_data = nullptr;
};