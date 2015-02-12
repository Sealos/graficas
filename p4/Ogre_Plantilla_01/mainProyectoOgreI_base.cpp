#include "Ogre\ExampleApplication.h"
Ogre::SceneNode *torretas [8];
Ogre::SceneNode *helices[2];

float helicesLoc[2][3] = {
	{-4904, 593, -23645}, {15151, 321, -23728}
};

float bordesSuperiores[2] = {1368, -1229};

class TorretasFrameListener : public FrameListener {
public:
	Ogre::Timer laserTimes[8];
	unsigned int tiempos[8];

	TorretasFrameListener() {
		//Hacer RANDOM
		for (int i = 0; i < 8; ++i) {
			laserTimes[i].reset();
			tiempos[i] = 2000;
		}
	}

	bool frameStarted(const Ogre::FrameEvent &evt) {
		for (int i = 0; i < 8; ++i) {
			if (laserTimes[i].getMilliseconds() > tiempos[i]) {
				laserTimes[i].reset();
				std::cout << "BAM!\n";
				//DISPARAR LASER
			}
		}

		return true;
	}
};

inline SceneNode *crearTorreta(SceneManager *mSceneMgr, Vector3 vec) {
	Ogre::Entity *ent = mSceneMgr->createEntity("usb_cilindro.mesh");
	Ogre::SceneNode *nodo = mSceneMgr->createSceneNode();
	nodo->attachObject(ent);
	nodo->translate(vec);
	return nodo;
}

ManualObject *helice(SceneManager *mSceneMgr) {
	ManualObject * h = mSceneMgr->createManualObject();
	h->begin("Examples/Chrome", RenderOperation::OT_TRIANGLE_FAN);
 
	// define vertex position of index 0..3
	h->position(-100.0, -100.0, 0.0);
	h->position( 100.0, -100.0, 0.0);
	h->position( 100.0,  100.0, 0.0);
	h->position(-100.0,  100.0, 0.0);
 
	// define usage of vertices by refering to the indexes
	h->index(0);
	h->index(1);
	h->index(2);
	h->index(3);
	h->index(0);
 
	h->end();
	return h;
}

inline SceneNode *crearHelice(SceneManager *mSceneMgr, Vector3 vec) {
	Ogre::SceneNode *nodoRaiz = mSceneMgr->createSceneNode();

	Ogre::Entity *ent = mSceneMgr->createEntity("usb_formacurva.mesh");
	Ogre::SceneNode *centro = mSceneMgr->createSceneNode();
	centro->attachObject(ent);

	Ogre::SceneNode *nodoH = mSceneMgr->createSceneNode();
	ManualObject *h = helice(mSceneMgr);
	nodoH->translate(100, 100, 100);
	nodoH->attachObject(h);

	nodoRaiz->addChild(centro);
	nodoRaiz->addChild(nodoH);
	nodoRaiz->scale(10, 10, 10);
	nodoRaiz->translate(vec);
	return nodoRaiz;
}

class Example25FrameListener : public FrameListener {
private:
	Ogre::SceneNode *_playerNode;
	Ogre::SceneNode *_padreNode;
	Ogre::Camera *_cam;
	OIS::InputManager *_man;
	OIS::Keyboard *_key;
	OIS::Mouse *_mouse;


public:

	Example25FrameListener(Ogre::SceneNode *playerNode, Ogre::SceneNode *padre, RenderWindow *win, Ogre::Camera *cam) {
		_playerNode = playerNode;
		_padreNode = padre;
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
		_key = static_cast<OIS::Keyboard *>(_man->createInputObject(OIS::OISKeyboard, false));
		//Camera assignment
		_mouse = static_cast<OIS::Mouse *>(_man->createInputObject(OIS::OISMouse, false));
		_cam = cam;
	}

	~Example25FrameListener() {
		_man->destroyInputObject(_key);
		_man->destroyInputObject(_mouse);
		OIS::InputManager::destroyInputSystem(_man);
	}

	bool frameStarted(const Ogre::FrameEvent &evt) {
		Ogre::Vector3 translateCam(0.0, 0.0, 0.0);
		Ogre::Vector3 translatePlayer(0.0, 0.0, 0.0);
		Ogre::Quaternion rotatePlayer(Degree(0), Vector3::UNIT_Y);
		Ogre::Quaternion strafePlayer(Degree(0), Vector3::UNIT_Z);
		float speedFactor = 2000.0f;
		float playerSpeed = 1000.f;
		float rotationSpeed = 50.f;
		float rotationAmount = 1.f;
		float strafeRotSpeed = 200.f;
		bool reiniciarPosicion = false;
		_key->capture();
		_mouse->capture();

		if (_key->isKeyDown(OIS::KC_ESCAPE))
			return false;

		if (_key->isKeyDown(OIS::KC_1))
			std::cout << _padreNode->getPosition() << "\n";

		Ogre::Quaternion q;
		//Player Controls

		// Move player UP
		if (_key->isKeyDown(OIS::KC_UP))
			translatePlayer += Ogre::Vector3(0.0, 1.0, 0.0);

		// Move player DOWN
		if (_key->isKeyDown(OIS::KC_DOWN))
			translatePlayer += Ogre::Vector3(0.0, -1.0, 0.0);

		// Rotate player LEFT
		if (_key->isKeyDown(OIS::KC_LEFT))
			rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree((rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Y);

		// Rotate player RIGHT
		if (_key->isKeyDown(OIS::KC_RIGHT))
			rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(-(rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Y);

		q = _playerNode->_getDerivedOrientation();
		Ogre::Radian angulo = q.getYaw();

		// Move foward
		if (_key->isKeyDown(OIS::KC_W))
			translatePlayer += Ogre::Vector3(-sin(angulo.valueRadians()), 0.0, -cos(angulo.valueRadians()));

		// Strafe Left
		if (_key->isKeyDown(OIS::KC_A))
			translatePlayer += Ogre::Vector3(-cos(angulo.valueRadians()), 0.0, sin(angulo.valueRadians()));

		// Strafe Right
		if (_key->isKeyDown(OIS::KC_D))
			translatePlayer += Ogre::Vector3(cos(angulo.valueRadians()), 0.0, -sin(angulo.valueRadians()));

		bool keyA = _key->isKeyDown(OIS::KC_A);
		bool keyD = _key->isKeyDown(OIS::KC_D);
		int sign;

		// Strafe left
		if (keyA && !keyD) {
			if (_playerNode->getOrientation().getRoll().valueDegrees() < 45.f)
				strafePlayer = strafePlayer * Ogre::Quaternion(Degree((rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Z);

		} else if (keyD && !keyA) {
			if (_playerNode->getOrientation().getRoll().valueDegrees() > -45.f)
				strafePlayer = strafePlayer * Ogre::Quaternion(Degree(-(rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Z);

		} else {
			if (_playerNode->getOrientation().getRoll().valueDegrees() > 0.f)
				sign = -1;
			else if (_playerNode->getOrientation().getRoll().valueDegrees() == 0.0f)
				sign = 0;
			else
				sign = 1;

			strafePlayer = strafePlayer * Ogre::Quaternion(Degree(sign * (rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Z);
		}

		_padreNode->translate(translatePlayer * evt.timeSinceLastFrame * playerSpeed);
		_padreNode->rotate(rotatePlayer);
		_playerNode->rotate(strafePlayer);
		return true;
	}
};

class Example1 : public ExampleApplication {

public:
	Ogre::FrameListener *FrameListener;
	Ogre::FrameListener *TorretaListener;
	Ogre::SceneNode *player;
	Ogre::SceneNode *padre;
	Ogre::SceneNode *cameraNode;

	Example1() {
		FrameListener = nullptr;
		TorretaListener = nullptr;
	}

	~Example1() {
		if (FrameListener)
			delete FrameListener;

		if (TorretaListener)
			delete TorretaListener;
	}

	void createFrameListener() {
		FrameListener = new Example25FrameListener(player, padre, mWindow, mCamera);
		TorretaListener = new TorretasFrameListener();
		mRoot->addFrameListener(FrameListener);
		//mRoot->addFrameListener(TorretaListener);
	}

	void createCamera() {
		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(0, 50, 300);
		mCamera->lookAt(0, 0, 0);
		mCamera->setNearClipDistance(5);
	}

	void createScene() {
		cameraNode = mSceneMgr->createSceneNode("CameraNodo");
		cameraNode->attachObject(mCamera);
		mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		Ogre::Entity *ent01 = mSceneMgr->createEntity("MyEntity1", "ejes01.mesh");
		Ogre::SceneNode *node01 = mSceneMgr->createSceneNode("Node01");
		mSceneMgr->getRootSceneNode()->addChild(node01);
		node01->attachObject(ent01);
		Ogre::Entity *entEscenario01 = mSceneMgr->createEntity("EscenarioBase01", "proyectoOgreI.mesh");
		Ogre::SceneNode *nodeEscenario01 = mSceneMgr->createSceneNode("NodeMesh01");
		mSceneMgr->getRootSceneNode()->addChild(nodeEscenario01);
		nodeEscenario01->attachObject(entEscenario01);
		Ogre::Entity *torus = mSceneMgr->createEntity("ObjetoPrueba", "usb_torus.mesh");
		Ogre::Entity *cilindro =  mSceneMgr->createEntity("ObjetoPrueba1", "RZR-002.mesh");
		player = mSceneMgr->createSceneNode("player");
		mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
		player->showBoundingBox(true);
		player->attachObject(torus);
		player->attachObject(cilindro);
		player->scale(3.0, 3.0, 3.0);
		//Ogre::SceneNode* nodo = crearTorreta(mSceneMgr,Ogre::Vector3(0.0,0.0,0.0));
		//mSceneMgr->getRootSceneNode()->addChild(nodo);
		padre = mSceneMgr->createSceneNode();
		padre->addChild(cameraNode);
		padre-> addChild(player);
		SceneNode *helice = crearHelice(mSceneMgr, Vector3(0.0, 0.0, 0.0));
		mSceneMgr->getRootSceneNode()->addChild(helice);
		//padre->rotate(Ogre::Quaternion(Degree(90), Vector3::UNIT_Y));
		mSceneMgr->getRootSceneNode()->addChild(padre);
	}

};


int main (void) {
	Example1 app;
	app.go();
	return 0;
}