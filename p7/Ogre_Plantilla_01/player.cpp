#include "stdafx.h"
#include "player.h"

Player::Player(SceneManager *sm, RenderWindow *win, SceneNode *_padreNode)
	: mSceneMgr(sm), _padreNode(_padreNode) {
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
	Entity *torus = mSceneMgr->createEntity("ObjetoPrueba", "usb_torus.mesh");
	torus->setMaterialName("lambert1");
	_playerNode = mSceneMgr->createSceneNode();
	_playerNode->showBoundingBox(true);
	_playerNode->attachObject(torus);
	_playerNode->scale(3.0, 3.0, 3.0);
	_padreNode->addChild(_playerNode);
}

Player::~Player() {
	_man->destroyInputObject(keyboard);
	OIS::InputManager::destroyInputSystem(_man);
	_padreNode->removeAndDestroyAllChildren();
	mSceneMgr->destroySceneNode(_padreNode);
}

bool Player::onUpdate(Real dtime) {
	Vector3 translateCam(0.0, 0.0, 0.0);
	Vector3 translatePlayer(0.0, 0.0, 0.0);
	Quaternion rotatePlayer(Degree(0), Vector3::UNIT_Y);
	Quaternion strafePlayer(Degree(0), Vector3::UNIT_Z);
	float speedFactor = 2000.0f;
	float playerSpeed = 1000.f;
	float rotationSpeed = 50.f;
	float rotationAmount = 1.f;
	float strafeRotSpeed = 200.f;
	bool reiniciarPosicion = false;
	keyboard->capture();

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
		rotatePlayer = rotatePlayer * Quaternion(Degree((rotationAmount * rotationSpeed * dtime)), Vector3::UNIT_Y);

	// Rotate player RIGHT
	if (keyboard->isKeyDown(OIS::KC_RIGHT))
		rotatePlayer = rotatePlayer * Quaternion(Degree(-(rotationAmount * rotationSpeed * dtime)), Vector3::UNIT_Y);

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
			strafePlayer = strafePlayer * Quaternion(Degree((rotationAmount * rotationSpeed * dtime)), Vector3::UNIT_Z);

	} else if (keyD && !keyA) {
		if (_playerNode->getOrientation().getRoll().valueDegrees() > -45.f)
			strafePlayer = strafePlayer * Quaternion(Degree(-(rotationAmount * rotationSpeed * dtime)), Vector3::UNIT_Z);

	} else {
		if (_playerNode->getOrientation().getRoll().valueDegrees() > 0.2f)
			sign = -1;
		else if (_playerNode->getOrientation().getRoll().valueDegrees() < -0.2f)
			sign = 1;
		else
			sign = 0;

		strafePlayer = strafePlayer * Quaternion(Degree(sign * (rotationAmount * rotationSpeed * dtime)), Vector3::UNIT_Z);
	}

	Vector3 top = _playerNode->_getWorldAABB().getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_TOP);
	Vector3 bot = _playerNode->_getWorldAABB().getCorner(AxisAlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM);
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

	if (deltaY > 0.0f && newTop > y_border[0])
		translatePlayer.y = 0.0;
	else if (deltaY < 0.0f && newBottom < y_border[1])
		translatePlayer.y = 0.0;

	if (deltaX > 0.0f && newRight > x_border[0])
		translatePlayer.x = 0.0;
	else if (deltaX < 0.0f && newLeft < x_border[1])
		translatePlayer.x = 0.0;

	// TODO(sdecolli): Revisar el near y far
	if (deltaZ > 0.0f && newNear > z_border[1])
		translatePlayer.z = 0.0;
	else if (deltaZ < 0.0f && newFar < z_border[0])
		translatePlayer.z = 0.0;

	_padreNode->translate(translatePlayer * dtime * playerSpeed);
	_padreNode->rotate(rotatePlayer);
	_playerNode->rotate(strafePlayer);

	return true;
}