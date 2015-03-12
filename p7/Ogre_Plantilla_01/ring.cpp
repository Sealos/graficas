#include "stdafx.h"
#include "ring.h"

const Real rotationSpeed = 100.f;

Real calculateRadius(SceneNode* _node)
{
	AxisAlignedBox myAABB = _node->_getWorldAABB();
	Vector3 myCenter = _node->getPosition();
	Vector3 corner1 = myAABB.getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_TOP);
	Vector3 corner2 = myAABB.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_BOTTOM);
	Vector3 medium(corner2.x, (corner2.y + corner1.y)/2 + corner2.y, (corner2.z + corner1.z)/2 + corner2.z);

	Real radius = medium.distance(myCenter);

	return radius;
}

inline bool Ring::isInCircle(Vector3 point)
{
	Vector3 myPos = _node->getPosition();
	Real x = point.x - myPos.x;
	Real y = point.y - myPos.y;
	return (x*x + y*y) < radius*radius;
}

bool Ring::isInCircle(AxisAlignedBox& bbox, Vector3 center) {
	Vector3 myCenter = _node->getPosition();
	Vector3 topRight = bbox.getCorner(AxisAlignedBox::CornerEnum::FAR_RIGHT_TOP);
	Vector3 botLeft = bbox.getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_BOTTOM);

	if (abs(center.z - myCenter.z) > 30.f)
		return false;

	if (!isInCircle(topRight) || !isInCircle(botLeft))
		return false;

	return true;
}

bool Ring::isColliding(AxisAlignedBox& bbox) {
	return _node->_getWorldAABB().intersects(bbox);
}

Ring::Ring(SceneNode *_node, SceneManager *mSceneMgr)
	: _node(_node), active(true), mSceneMgr(mSceneMgr)
{
	_node->showBoundingBox(true);
	radius = calculateRadius(_node);
}