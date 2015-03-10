#pragma once

class Ring {
public:
	bool active;
	SceneNode *_node;
	Ring(){};
	Ring(SceneNode *);
	~Ring(){};

	void onUpdate(Real);
	bool isColliding(Vector3);
	bool isColliding(AxisAlignedBox& bbox);
};