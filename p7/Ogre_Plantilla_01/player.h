#pragma once
#include "coin.h"

const Real x_border[2] = {10000.0f, -10000.0f};
const Real y_border[2] = {5000.0f, -5000.0f};
const Real z_border[2] = {90000.0f, -100.0f};

class Player {
public:
	SceneManager *mSceneMgr;
	OIS::Keyboard *keyboard;
	SceneNode *_playerNode;
	SceneNode *_padreNode;
	OIS::InputManager *_man;

	bool onUpdate(Real);
	void createPlayerNode();
	Player() {};
	~Player();
	Player(SceneManager *, RenderWindow *, SceneNode *); 
	bool onCollisionWithCoin(Coin *);
};
