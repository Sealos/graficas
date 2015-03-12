#pragma once

class Ring {
public:
	bool active;
	SceneNode *_node;
	SceneManager *mSceneMgr;
	Ring(){};
	Ring(SceneNode *, SceneManager *);
	~Ring(){};

	void onUpdate(Real);
	bool isColliding(Vector3);
	bool isColliding(AxisAlignedBox& bbox);
};