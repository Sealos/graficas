#pragma once

class Ring {
public:
	SceneNode *_node;
	Ring(){};
	Ring(SceneNode *);
	~Ring();

	void onUpdate(Real);
	bool isColliding(Vector3);
};