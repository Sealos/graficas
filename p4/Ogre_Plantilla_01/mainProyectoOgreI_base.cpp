#include "Ogre\ExampleApplication.h"
#include <vector>
Ogre::SceneNode *torretas [8];
Ogre::SceneNode *helices[2];

Ogre::Real ultimaFila = -23354;
Ogre::Real penultimaFila = -19095;
Ogre::Real altura = -332;

Ogre::Vector3 posicionesT[8] = {
	Ogre::Vector3(-1615, altura, -8573),
	Ogre::Vector3(1615, altura, -15240),
	Ogre::Vector3(8135, altura, ultimaFila),
	Ogre::Vector3(15296, altura, penultimaFila),
	Ogre::Vector3(23894, altura, ultimaFila),
	Ogre::Vector3(-10229, altura, ultimaFila),
	Ogre::Vector3(-16939, altura, penultimaFila),
	Ogre::Vector3(-23027, altura, ultimaFila)
};

Ogre::SceneManager *mainSceneMgr;

class Laser {
public:
	Ogre::SceneNode *laserNodo;
	Ogre::Entity *laserEnt;
	Ogre::Vector3 direccionP;
	float velocidad;

	Laser() {
		laserNodo = nullptr;
		laserEnt = nullptr;
		velocidad = 1.f;
	}

	Laser(Ogre::Vector3 vec, Ogre::Vector3 playerDir) {
		laserEnt = mainSceneMgr -> createEntity("usb_laser.mesh");
		laserNodo = mainSceneMgr -> createSceneNode();
		laserNodo -> attachObject(laserEnt);
		Ogre::Vector3 src = laserNodo -> getOrientation() * Vector3::UNIT_Y;
		Ogre::Quaternion dir = src.getRotationTo(-playerDir);
		laserNodo -> rotate(dir);
		laserNodo -> translate(vec);
		direccionP = playerDir;
		velocidad = 25.f;
	}

	void move() {
		laserNodo -> translate(direccionP * velocidad);
	}

	~Laser() {
		if (laserNodo) {
			delete laserNodo;
		}
		if (laserEnt) {
			delete laserEnt;
		}
	}

};

std::vector<Laser *> laseres;

float helicesLoc[2][3] = {
	{ -13315, 321, -23728}, {15151, 321, -23728}
};

float bordesSuperiores[2] = {1368, -1229};


class TorretasFrameListener : public FrameListener {
public:
	Ogre::SceneNode *_playerNode;
	Ogre::Timer laserTimes[8];
	unsigned int tiempos[8];

	TorretasFrameListener(SceneNode *player) {
		_playerNode = player;
		//Hacer RANDOM
		for (int i = 0; i < 8; ++i) {
			laserTimes[i].reset();
			tiempos[i] = 2000;
		}
	}

	bool frameStarted(const Ogre::FrameEvent &evt) {
		for (int i = 0; i < 8; ++i) {
			if (laserTimes[i].getMilliseconds() > tiempos[i]) {
				Ogre::Vector3 playerDirection = posicionesT[i] - _playerNode -> getPosition();
				Ogre::Real distance = playerDirection.normalise();
				laserTimes[i].reset();
				Laser *las = new Laser(posicionesT[i], -playerDirection);
				mainSceneMgr -> getRootSceneNode() -> addChild(las -> laserNodo);
				laseres.push_back(las);
				//DISPARAR LASER
				std::cout << "BAM\n";
			}
		}

		for (unsigned int i = 0; i < laseres.size(); ++i) {
			laseres[i] -> move();
		}
		return true;
	}
};

class HelicesFrameListener : public FrameListener {
public:
	Ogre::Timer time;
	unsigned int tiempos[8];

	HelicesFrameListener() {
		//Hacer RANDOM
		time.reset();
	}

	bool frameStarted(const Ogre::FrameEvent &evt) {
		if (time.getMilliseconds() > 50) {
			Quaternion q(Degree(evt.timeSinceLastFrame * 20), Vector3::UNIT_Z);
			helices[0]->rotate(q);
			helices[1]->rotate(q);
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

inline SceneNode *crearHelice(SceneManager *mSceneMgr, Vector3 vec) {
	Ogre::SceneNode *nodoRaiz = mSceneMgr->createSceneNode();

	Ogre::Entity *ent;
	Ogre::SceneNode *nodo;

	nodo = mSceneMgr->createSceneNode();
	ent = mSceneMgr->createEntity("spine.mesh");
	ent->setMaterialName("Examples/GrassFloor");
	nodo->attachObject(ent);
	nodoRaiz->addChild(nodo);

	nodo = mSceneMgr->createSceneNode();
	ent = mSceneMgr->createEntity("spine.mesh");
	ent->setMaterialName("Examples/GrassFloor");
	nodo->attachObject(ent);
	nodo->rotate(Quaternion(Degree(180.0), Vector3::UNIT_Z));
	nodoRaiz->addChild(nodo);

	nodo = mSceneMgr->createSceneNode();
	ent = mSceneMgr->createEntity("spine.mesh");
	ent->setMaterialName("Examples/GrassFloor");
	nodo->attachObject(ent);
	nodo->rotate(Quaternion(Degree(270.0), Vector3::UNIT_Z));
	nodoRaiz->addChild(nodo);

	nodo = mSceneMgr->createSceneNode();
	ent = mSceneMgr->createEntity("spine.mesh");
	ent->setMaterialName("Examples/GrassFloor");
	nodo->attachObject(ent);
	nodo->rotate(Quaternion(Degree(90.0), Vector3::UNIT_Z));
	nodoRaiz->addChild(nodo);

	nodoRaiz->scale(35, 35, 35);
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
	Ogre::FrameListener *HeliceListener;
	Ogre::SceneNode *player;
	Ogre::SceneNode *padre;
	Ogre::SceneNode *cameraNode;

	Example1() {
		FrameListener = nullptr;
		TorretaListener = nullptr;
		HeliceListener = nullptr;
	}

	~Example1() {
		if (FrameListener)
			delete FrameListener;

		if (TorretaListener)
			delete TorretaListener;

		if (HeliceListener)
			delete HeliceListener;
	}

	void createFrameListener() {
		FrameListener = new Example25FrameListener(player, padre, mWindow, mCamera);
		TorretaListener = new TorretasFrameListener(padre);
		HeliceListener = new HelicesFrameListener();
		mRoot -> addFrameListener(FrameListener);
		mRoot -> addFrameListener(TorretaListener);
		mRoot->addFrameListener(HeliceListener);


	}



	void createCamera() {
		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(0, 50, 300);
		mCamera->lookAt(0, 0, 0);

		mCamera->setNearClipDistance(5);
	}


	void chooseSceneManager() {
		// Create the SceneManager, in this case a generic one
		mainSceneMgr = mRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");
		mSceneMgr = mainSceneMgr;

		if (mOverlaySystem)
			mSceneMgr->addRenderQueueListener(mOverlaySystem);
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
		Ogre::Entity *cilindro =  mSceneMgr->createEntity("ObjetoPrueba1", "usb_cubomod01.mesh");
		torus -> setMaterialName("lambert3");
		player = mSceneMgr -> createSceneNode("player");
		player -> showBoundingBox(true);





		player -> attachObject(torus);
		player -> attachObject(cilindro);
		player -> scale(3.0, 3.0, 3.0);

		for (int i = 0 ; i < 8 ; ++i) {
			torretas[i] = crearTorreta(mSceneMgr, posicionesT[i]);
			mSceneMgr -> getRootSceneNode() -> addChild(torretas[i]);
		}


		padre = mSceneMgr -> createSceneNode();
		padre -> addChild(cameraNode);

		mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

		padre-> addChild(player);
		SceneNode *helice = crearHelice(mSceneMgr, Vector3(helicesLoc[0][0], helicesLoc[0][1], helicesLoc[0][2]));
		helices[0] = helice;
		helice->rotate(Quaternion(Degree(90), Vector3::UNIT_Y));
		mSceneMgr->getRootSceneNode()->addChild(helice);
		helice = crearHelice(mSceneMgr, Vector3(helicesLoc[1][0], helicesLoc[1][1], helicesLoc[1][2]));
		helices[1] = helice;
		helice->rotate(Quaternion(Degree(90), Vector3::UNIT_Y));
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