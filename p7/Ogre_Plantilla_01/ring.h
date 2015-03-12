#pragma once

class Ring {
public:
	bool active;
	Real radius;
	SceneNode *_node;
	SceneManager *mSceneMgr;
	Ring(){};
	Ring(SceneNode *, SceneManager *);
	~Ring(){};

	void onUpdate(Real){};
	bool isInCircle(AxisAlignedBox&, Vector3);
	bool isInCircle(Vector3);
	bool isColliding(AxisAlignedBox& bbox);
};