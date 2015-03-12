#include "stdafx.h"
#include "player.h"

Player::Player(SceneManager *sm, RenderWindow *win, SceneNode *_padreNode)
	: mSceneMgr(sm), _padreNode(_padreNode), health(3), score(0) {
	//Helper variables for extracting the window handle
	size_t windowHnd = 0;
	std::stringstream windowHndStr;
	win->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	//Create the parameter list for OIS and add the handle 'windowHndStr' to it
	OIS::ParamList pl;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	//Create the input system
	_man = OIS::InputManager::createInputSystem(pl);
	keyboard = static_cast<OIS::Keyboard *>(_man->createInputObject(OIS::OISKeyboard, false));
	createPlayerNode();
}

void Player::createPlayerNode() {
	Entity *torus = mSceneMgr->createEntity("ObjetoPrueba", "ship.mesh");
	torus->setMaterialName("lambert1");
	_playerNode = mSceneMgr->createSceneNode();
	_playerNode->showBoundingBox(true);
	_playerNode->attachObject(torus);
	_playerNode->scale(12.0, 6.0, 12.0);
	_padreNode->rotate(Quaternion(Degree(180.f), Vector3::UNIT_Y));
	_padreNode->addChild(_playerNode);
}

void Player::reset() {
	health = 3;
	score = 0;
	_padreNode->setPosition(0.0f, 0.0f, 0.0f);
	_padreNode->setOrientation(Quaternion(Degree(180.0f), Vector3::UNIT_Y));
}

Player::~Player() {
	_man->destroyInputObject(keyboard);
	OIS::InputManager::destroyInputSystem(_man);
	_padreNode->removeAndDestroyAllChildren();
	mSceneMgr->destroySceneNode(_padreNode);
}

inline bool getBlinkingVisibility(Real decimal)
{
	if (decimal > 0.75f || (decimal < 0.5f && decimal > 0.25f))
		return false;
	return true;
}

bool Player::onUpdate(Real dtime) {
	Vector3 translateCam(0.0f, 0.0f, 0.0f);
	Vector3 translatePlayer(0.0f, 0.0f, 0.0f);
	Quaternion rotatePlayer(Degree(0), Vector3::UNIT_Y);
	Quaternion strafePlayer(Degree(0), Vector3::UNIT_Z);
	float playerSpeed = 3000.f;
	float rotationSpeed = 100.f;
	float strafeRotSpeed = 200.f;
	bool reiniciarPosicion = false;
	keyboard->capture();

	if (invincibilityTime > 0.0f)
		invincibilityTime -= dtime;
	else
		invincibilityTime = 0.0f;

	Real yup;
	Real decimal = modf(invincibilityTime, &yup);
	_playerNode->setVisible(getBlinkingVisibility(decimal));

	if (keyboard->isKeyDown(OIS::KC_ESCAPE))
		return false;

	if (keyboard->isKeyDown(OIS::KC_1))
		std::cout << _padreNode->getPosition() << "\n";

	Quaternion q;
	//Player Controls

	// Move player UP
	if (keyboard->isKeyDown(OIS::KC_UP))
		translatePlayer += Vector3(0.0, 1.0, 0.0);

	// Move player DOWN
	if (keyboard->isKeyDown(OIS::KC_DOWN))
		translatePlayer += Vector3(0.0, -1.0, 0.0);

	// Rotate player LEFT
	if (keyboard->isKeyDown(OIS::KC_LEFT))
		rotatePlayer = rotatePlayer * Quaternion(Degree((rotationSpeed * dtime)), Vector3::UNIT_Y);

	// Rotate player RIGHT
	if (keyboard->isKeyDown(OIS::KC_RIGHT))
		rotatePlayer = rotatePlayer * Quaternion(Degree(-(rotationSpeed * dtime)), Vector3::UNIT_Y);

	q = _playerNode->_getDerivedOrientation();
	Radian angulo = q.getYaw();

	// Move foward
	if (keyboard->isKeyDown(OIS::KC_W))
		translatePlayer += Vector3(-sin(angulo.valueRadians()), 0.0, -cos(angulo.valueRadians()));

	// Strafe Left
	if (keyboard->isKeyDown(OIS::KC_A))
		translatePlayer += Vector3(-cos(angulo.valueRadians()), 0.0, sin(angulo.valueRadians()));

	// Strafe Right
	if (keyboard->isKeyDown(OIS::KC_D))
		translatePlayer += Vector3(cos(angulo.valueRadians()), 0.0, -sin(angulo.valueRadians()));

	bool keyA = keyboard->isKeyDown(OIS::KC_A);
	bool keyD = keyboard->isKeyDown(OIS::KC_D);
	int sign;

	// Strafe left
	if (keyA && !keyD) {
		if (_playerNode->getOrientation().getRoll().valueDegrees() < 45.f)
			strafePlayer = strafePlayer * Quaternion(Degree(( rotationSpeed * dtime)), Vector3::UNIT_Z);

	} else if (keyD && !keyA) {
		if (_playerNode->getOrientation().getRoll().valueDegrees() > -45.f)
			strafePlayer = strafePlayer * Quaternion(Degree(-(rotationSpeed * dtime)), Vector3::UNIT_Z);

	} else {
		if (_playerNode->getOrientation().getRoll().valueDegrees() > 0.2f)
			sign = -1;
		else if (_playerNode->getOrientation().getRoll().valueDegrees() < -0.2f)
			sign = 1;
		else
			sign = 0;

		strafePlayer = strafePlayer * Quaternion(Degree(sign * (rotationSpeed * dtime)), Vector3::UNIT_Z);
	}

	AxisAlignedBox aabb = _playerNode->_getWorldAABB();
	Vector3 top = aabb.getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_TOP);
	Vector3 bot = aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM);
	Vector3 leftFar = top;
	Vector3 rightNear = bot;
	Vector3 oldPosition = _padreNode->getPosition();
	Vector3 newPosition = translatePlayer * dtime * playerSpeed + _padreNode->getPosition();
	Real deltaY = newPosition.y - oldPosition.y;
	Real deltaX = newPosition.x - oldPosition.x;
	Real deltaZ = newPosition.z - oldPosition.z;
	Real newTop = top.y + deltaY;
	Real newBottom = bot.y + deltaY;
	Real newLeft = leftFar.x + deltaX;
	Real newRight = rightNear.x + deltaX;
	Real newFar = leftFar.z + deltaZ;
	Real newNear = rightNear.z + deltaZ;

	if (newPosition.z > 89500)
	{
		reset();
		return true;
	}

	if (deltaY > 0.0f && newTop > y_border[0])
		translatePlayer.y = 0.0;

	else if (deltaY < 0.0f && newBottom < y_border[1])
		translatePlayer.y = 0.0;

	if (deltaX > 0.0f && newRight > x_border[0])
		translatePlayer.x = 0.0;

	else if (deltaX < 0.0f && newLeft < x_border[1])
		translatePlayer.x = 0.0;

	if (deltaZ < 0.0f && newNear < z_border[1])
		translatePlayer.z = 0.0;

	else if (deltaZ > 0.0f && newFar > z_border[0])
		translatePlayer.z = 0.0;

	_padreNode->translate(translatePlayer * dtime * playerSpeed);
	_padreNode->rotate(rotatePlayer);
	_playerNode->rotate(strafePlayer);
	return true;
}

void Player::checkCollision(Ring& ring)
{
	AxisAlignedBox aabb = _playerNode->_getWorldAABB();
	Vector3 point = _padreNode->getPosition();
	if (ring.active && ring.isInCircle(aabb, point) && ring.isColliding(aabb))
		onCollision(ring);
}

void Player::onCollision(Ring& ring)
{
	score += 200;
	ring.active = false;
	Entity *ringEnt = static_cast<Entity*>(ring._node->getAttachedObject(0));
	ringEnt->setMaterialName("Examples/Chrome");
	std::cout << "Puntaje: " << score << std::endl;
}

void Player::checkCollision(Coin& coin)
{
	AxisAlignedBox aabb = _playerNode->_getWorldAABB();
	if (coin.active && coin.isColliding(aabb))
		onCollision(coin);
}

void Player::onCollision(Coin& coin)
{
	score += 100;
	coin.active = false;
	coin._node->setVisible(false);
	std::cout << "Puntaje: " << score << std::endl;
}

inline void Player::dealDamage()
{
	std::cout << "Current health: " << --health << std::endl;
	invincibilityTime = 3.0f;
}

void Player::checkCollision(Obstacle& obs)
{
	AxisAlignedBox aabb = _playerNode->_getWorldAABB();
	if (health > 0 && obs.isColliding(aabb) && invincibilityTime <= 0)
		onCollision(obs);
}

inline void Player::onCollision(Obstacle& obs)
{
	dealDamage();
}