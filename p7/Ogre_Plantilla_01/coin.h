#pragma once

class Coin {
public:
	SceneNode *_node;
	Coin(){};
	Coin(SceneNode *);
	~Coin(){};

	void onUpdate(Real);
	bool isColliding(Vector3);
};