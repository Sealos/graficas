#pragma once

class Obstacle {
public:
	SceneNode *_node;
	Vector3 rotationVector;
	Real rotationSpeed;

	Obstacle(){};
	Obstacle(SceneNode *, Vector3, Real rotationSpeed = 100.f);
	~Obstacle(){};
	void onUpdate(Real);
	bool isColliding(AxisAlignedBox&);
};
