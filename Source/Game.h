#pragma once
#include <Engine/OGLGame.h>
#include <string>

#include "GameData.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Blockade.h"
#include "Explosion.h"


struct GameFont;


/**
*  Invaders Game. An OpenGL Game based on ASGE.
*/


class InvadersGame:
	public ASGE::OGLGame
{
public:
	InvadersGame();
	~InvadersGame();

	

	virtual bool init();
	virtual bool initGameplay();
	virtual bool initPause();
	virtual bool initGameover();
	virtual bool run() override;
	bool shouldExit() const;
	void render();
	virtual void drawFrame();
	virtual void drawFrameGame();
	virtual void drawFramePause();
	virtual void drawFrameGameover();
	virtual void resetLevel();
	void input(int key, int action) const;
	void processGameActions();
	virtual void checkCollisions();
	void initBlockades();
	virtual void initSounds();

private:
	int  callback_id = -1;								/**< Input Callback ID. The callback ID assigned by the game engine. */
	bool exit = false;									/**< Exit boolean. If true the game loop will exit. */
	std::unique_ptr<GameData> game_data = nullptr;		/**< Game Data to pass around to objects. */
	std::unique_ptr<ASGE::Sprite> sprite = nullptr;		/**< Sprite Object. The space invader sprite. */
	std::unique_ptr<ASGE::Sprite> pause_screen = nullptr;
	std::unique_ptr<ASGE::Sprite> gameover_screen = nullptr;
	std::unique_ptr<Player> player_one = nullptr;
	std::unique_ptr<EnemyManager> enemy_man = nullptr;
	std::vector<std::unique_ptr<Blockade>> blockades;
	const int num_blockades = 5;
};

