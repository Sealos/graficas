#include "stdafx.h"
#include "obstacle.h"

Obstacle::Obstacle(SceneNode *_node, Vector3 rotationVector, Real rotationSpeed = 100.f)
	: _node(_node), rotationVector(rotationVector), rotationSpeed(rotationSpeed) {

}

void Obstacle::onUpdate(Real dtime) {
	Quaternion rotation(Degree(0), rotationVector);
	rotation = rotation * Quaternion(Degree((rotationSpeed * dtime)), rotationVector);
	_node->rotate(rotation);
}

bool Obstacle::isColliding(Vector3 point) {
	Vector3 farLeftTop = _node->_getWorldAABB().getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_TOP);
	Vector3 nearRightBottom = _node->_getWorldAABB().getCorner(AxisAlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM);

	if (point.x > nearRightBottom.x || point.x < farLeftTop.x)
		return false;

	if (point.y > farLeftTop.y || point.y < nearRightBottom.y)
		return false;

	if (point.z > nearRightBottom.z || point.z < farLeftTop.z)
		return false;

	return true;
}