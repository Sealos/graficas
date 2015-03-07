#include "stdafx.h"
#include "obstacle.h"

Obstacle::Obstacle(SceneNode *_node, Vector3 rotationVector, Real rotationSpeed)
	: _node(_node), rotationVector(rotationVector), rotationSpeed(rotationSpeed) {

}

void Obstacle::onUpdate(Real dtime) {
	Quaternion rotation(Degree(0), rotationVector);
	rotation = rotation * Quaternion(Degree((rotationSpeed * dtime)), rotationVector);
	_node->rotate(rotation);
}

bool Obstacle::isColliding(AxisAlignedBox& bbox) {
	return _node->_getWorldAABB().intersects(bbox);
}