#include "stdafx.h"
#include "coin.h"

const Real rotationSpeed = 100.f;

void Coin::onUpdate(Real dtime) {
	Quaternion rotation = Quaternion(Degree((rotationSpeed * dtime)), Vector3::UNIT_Z);
	_node->rotate(rotation);
}

Coin::Coin(SceneNode* m_node): active(true)
{
	_node = m_node;
}

bool Coin::isColliding(Vector3 point) {
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

bool Coin::isColliding(AxisAlignedBox& bbox) {
	return _node->_getWorldAABB().intersects(bbox);
}
