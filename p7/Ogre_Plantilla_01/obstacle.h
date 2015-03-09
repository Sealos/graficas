#pragma once

class Obstacle {
public:
	SceneNode *_node;
	Vector3 rotationVector;
	Vector3 movementVector;
	Real rotationSpeed;
	Real movementSpeed;
	int x_direction;
	int y_direction;
	int z_direction;

	Obstacle(){};
	Obstacle(SceneNode *, Vector3, Real rotationSpeed, Vector3, Real movementSpeed = 100.f);
	Obstacle(SceneNode *, Vector3, Real rotationSpeed = 100.f);
	Obstacle(SceneNode *);
	~Obstacle(){};
	virtual void onUpdate(Real);
	bool isColliding(AxisAlignedBox&);
};
