#pragma once
#include "coin.h"
#include "ring.h"
#include "obstacle.h"



class Player {
public:
	SceneManager *mSceneMgr;
	OIS::Keyboard *keyboard;
	SceneNode *_playerNode;
	SceneNode *_padreNode;
	OIS::InputManager *_man;


	size_t lives;

	bool onUpdate(Real);
	void createPlayerNode();
	Player() {};
	~Player();
	Player(SceneManager *, RenderWindow *, SceneNode *); 
	void onCollision(Coin*);
	void onCollision(Ring*);
	void onCollision(Obstacle*);
	void checkCollision(Coin*);
	void checkCollision(Ring*);
	void checkCollision(Obstacle*);
};
