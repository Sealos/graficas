#include "stdafx.h"
#include "obstacle.h"

Obstacle::Obstacle(SceneNode *_node, Vector3 rotationVector, Real rotationSpeed)
	: _node(_node), rotationVector(rotationVector), rotationSpeed(rotationSpeed) {
		x_direction = 1;
		y_direction = 1;
		z_direction = 1;
}

Obstacle::Obstacle(SceneNode *_node, Vector3 rotationVector, Real rotationSpeed, Vector3 movementDirection, Real movementSpeed)
	:	_node(_node), rotationVector(rotationVector), rotationSpeed(rotationSpeed), 
		movementVector(movementDirection), movementSpeed(movementSpeed) {
		x_direction = 1;
		y_direction = 1;
		z_direction = 1;
}

void Obstacle::onUpdate(Real dtime) {
	Quaternion rotation(Degree(0), rotationVector);
	rotation = rotation * Quaternion(Degree((rotationSpeed * dtime)), rotationVector);
	_node->rotate(rotation);
	
	Vector3 translate = Vector3(movementVector.x * x_direction, movementVector.y * y_direction, movementVector.z * z_direction);


	Vector3 top = _node->_getWorldAABB().getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_TOP);
	Vector3 bot = _node->_getWorldAABB().getCorner(AxisAlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM);
	Vector3 leftFar = top;
	Vector3 rightNear = bot;
	Vector3 oldPosition = _node->getPosition();
	Vector3 newPosition = translate * dtime * movementSpeed + _node->getPosition();
	Real deltaY = newPosition.y - oldPosition.y;
	Real deltaX = newPosition.x - oldPosition.x;
	Real deltaZ = newPosition.z - oldPosition.z;
	Real newTop = top.y + deltaY;
	Real newBottom = bot.y + deltaY;
	Real newLeft = leftFar.x + deltaX;
	Real newRight = rightNear.x + deltaX;
	Real newFar = leftFar.z + deltaZ;
	Real newNear = rightNear.z + deltaZ;

	if (deltaY > 0.0f && newTop > y_border[0]) {
		translate.y = 0.0;
		y_direction *= -1;
	}
	else if (deltaY < 0.0f && newBottom < y_border[1]) {
		translate.y = 0.0;
		y_direction *= -1;
	}

	if (deltaX > 0.0f && newRight > x_border[0]) {
		translate.x = 0.0;
		x_direction *= -1;
	}
	else if (deltaX < 0.0f && newLeft < x_border[1]) {
		translate.x = 0.0;
		x_direction *= -1;
	}

	// TODO(sdecolli): Revisar el near y far
	if (deltaZ > 0.0f && newNear > z_border[1]) {
		translate.z = 0.0;
		z_direction *= -1;
	}
	else if (deltaZ < 0.0f && newFar < z_border[0]) {
		translate.z = 0.0;
		z_direction *= -1;
	}

	_node->translate(translate * dtime * movementSpeed);

}

bool Obstacle::isColliding(AxisAlignedBox& bbox) {
	return _node->_getWorldAABB().intersects(bbox);
}

