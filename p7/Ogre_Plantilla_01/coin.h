#pragma once

class Coin {
public:
	SceneNode *_node;
	bool active;
	Coin(){};
	Coin(SceneNode *);
	~Coin(){};

	void onUpdate(Real);
	bool isColliding(Vector3);
	bool isColliding(AxisAlignedBox& bbox);
};