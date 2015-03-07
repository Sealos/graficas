#include "stdafx.h"
#include "ring.h"

Real rotationSpeed = 100.f;

void Ring::onUpdate(Real dtime) {
}

bool Ring::isColliding(Vector3 point) {
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