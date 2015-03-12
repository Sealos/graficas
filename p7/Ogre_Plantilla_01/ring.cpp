#include "stdafx.h"
#include "ring.h"

const Real rotationSpeed = 100.f;

Real calculateRadius(SceneNode* _node)
{
	AxisAlignedBox myAABB = _node->_getWorldAABB();
	Vector3 myCenter = _node->getPosition();
	Vector3 corner1 = myAABB.getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_TOP);
	Vector3 medium(corner1.x, myCenter.y, myCenter.z);

	return medium.distance(myCenter);
}

inline bool Ring::isInCircle(Vector3 point)
{
	Vector3 myPos = _node->getPosition();
	Real x = point.x - myPos.x;
	Real y = point.y - myPos.y;

	bool result = (x*x + y*y) < radius*radius;
	if (result)
		std::cout << "point.x: "<< point.x << " point.y " << point.y << std::endl;
	return result;
}

bool Ring::isInCircle(AxisAlignedBox& bbox, Vector3 center) {
	Vector3 myCenter = _node->getPosition();
	Vector3 topRight = bbox.getCorner(AxisAlignedBox::CornerEnum::FAR_RIGHT_TOP);
	Vector3 botLeft = bbox.getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_BOTTOM);

	if (abs(center.z - myCenter.z) > 20.f)
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

	std::cout << radius << std::endl;
}