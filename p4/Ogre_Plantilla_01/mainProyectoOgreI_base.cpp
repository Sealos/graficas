#include "stdafx.h"

SceneNode *torretas [8];
SceneNode *helices[2];
float alturaBarril;

float helicesLoc[2][3] = {
	{ -13315, 321, -23728}, {15151, 321, -23728}
};

float bordes[2] = {1368, -1229};

// x, z
float centroPasillos[2][2] = {
	{0.f, -9280.f}, {0.f, -21560.f}
};


// width, height
float medidasPasillos[2][2] = {
	{3500.f, 19000.f}, {52000.f, 4500.f}
};

float minMaxX[2][2] = {
	{centroPasillos[0][0] - medidasPasillos[0][0]/2, centroPasillos[0][0] + medidasPasillos[0][0]/2},

	{centroPasillos[1][0] - medidasPasillos[1][0]/2, centroPasillos[1][0] + medidasPasillos[1][0]/2},
};

float minMaxZ[2][2] = {
	{centroPasillos[0][1] + medidasPasillos[0][1]/2, centroPasillos[0][1] - medidasPasillos[0][1]/2},

	{centroPasillos[1][1] + medidasPasillos[1][1]/2, centroPasillos[1][1] - medidasPasillos[1][1]/2},
};

Real ultimaFila = -23354;
Real penultimaFila = -18900;
Real altura = -332;

Vector3 posicionesT[8] = {
	Vector3(-1615, altura, -8573),
	Vector3(1615, altura, -15240),
	Vector3(8135, altura, ultimaFila),
	Vector3(15400, altura, penultimaFila),
	Vector3(23894, altura, ultimaFila),
	Vector3(-10000, altura, ultimaFila),
	Vector3(-16939, altura, penultimaFila),
	Vector3(-23027, altura, ultimaFila)
};

SceneManager *mainSceneMgr;

class Laser {
public:
	SceneNode *laserNodo;
	Entity *laserEnt;
	Vector3 direccionP;
	float velocidad;

	Laser() {
		laserNodo = nullptr;
		laserEnt = nullptr;
		velocidad = 1.f;
	}

	Laser(Vector3 vec, Vector3 playerDir) {
		laserEnt = mainSceneMgr -> createEntity("usb_laser.mesh");
		laserNodo = mainSceneMgr -> createSceneNode();
		laserNodo -> attachObject(laserEnt);
		Vector3 src = laserNodo -> getOrientation() * Vector3::UNIT_Y;
		Quaternion dir = src.getRotationTo(-playerDir);
		laserNodo -> rotate(dir);
		laserNodo -> translate(vec);
		laserNodo -> scale(2.0,2.0,2.0);
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

class TorretasFrameListener : public FrameListener {
public:
	SceneNode *_playerNode;
	Timer laserTimes[8];
	unsigned int tiempos[8];

	TorretasFrameListener(SceneNode *player) {
		_playerNode = player;
		//Hacer RANDOM
		for (int i = 0; i < 8; ++i) {
			laserTimes[i].reset();
			tiempos[i] = 2000;
		}
	}

	bool frameStarted(const FrameEvent &evt) {
		for (int i = 0; i < 8; ++i) {
			if (laserTimes[i].getMilliseconds() > tiempos[i]) {
				Vector3 playerDirection = (posicionesT[i]+Vector3(0.0,alturaBarril,0.0)) - _playerNode -> getPosition();
				Real distance = playerDirection.normalise();
				laserTimes[i].reset();
				Laser *las = new Laser((posicionesT[i]+Vector3(0.0,alturaBarril,0.0)), -playerDirection);
				mainSceneMgr -> getRootSceneNode() -> addChild(las -> laserNodo);
				laseres.push_back(las);
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
	Timer time;
	unsigned int tiempos[8];

	HelicesFrameListener() {
		//Hacer RANDOM
		time.reset();
	}

	bool frameStarted(const FrameEvent &evt) {
		if (time.getMilliseconds() > 50) {
			Quaternion q(Degree(evt.timeSinceLastFrame * 20), Vector3::UNIT_Z);
			helices[0]->rotate(q);
			helices[1]->rotate(q);
		}
		return true;
	}
};

inline SceneNode *crearTorreta(SceneManager *mSceneMgr, Vector3 vec) {

	//Cargar entidades
	Ogre::Entity* base = mSceneMgr->createEntity("usb_cubomod01.mesh");
	Ogre::Entity* cuerpo = mSceneMgr-> createEntity("usb_cilindro.mesh");
	Ogre::Entity* tapa = mSceneMgr-> createEntity("usb_formacurva.mesh");
	Ogre::Entity* barril = mSceneMgr -> createEntity("usb_cilindro02.mesh");
	//Asignar Materiales
	base -> setMaterialName("lambert3");
	cuerpo -> setMaterialName("lambert3");
	tapa -> setMaterialName("lambert3");
	barril -> setMaterialName("lambert3");
	//Crear escenas
	Ogre::SceneNode *nodoBase = mSceneMgr->createSceneNode();
	Ogre::SceneNode *nodoCuerpo = mSceneMgr->createSceneNode();
	Ogre::SceneNode *nodoTapa = mSceneMgr->createSceneNode();
	SceneNode* nodoBarril = mSceneMgr-> createSceneNode();
	Ogre::SceneNode *nodo = mSceneMgr->createSceneNode();

	nodoBase -> attachObject(base);
	nodoCuerpo -> attachObject(cuerpo);
	nodoTapa -> attachObject(tapa);
	nodoBarril -> attachObject(barril);

	nodo -> addChild(nodoBase);
	nodo -> addChild(nodoCuerpo);
	nodo -> addChild(nodoTapa);
	nodo -> addChild(nodoBarril);

	nodoBase -> scale(3.0,2.0,3.0);
	nodoCuerpo -> scale(2.0,2.0,2.0);
	nodoBase -> _updateBounds();
	nodoCuerpo -> _updateBounds();
	nodoTapa -> _updateBounds();
	nodoBarril ->_updateBounds();

	Ogre::AxisAlignedBox aab = nodoBase -> _getWorldAABB();
	float alturaBase = aab.getSize().y;
	aab = nodoCuerpo -> _getWorldAABB();
	float alturaCuerpo = aab.getSize().y;
	aab =nodoCuerpo -> _getWorldAABB();
	float alturaTapa = aab.getSize().y;
	float centroXTapa = aab.getHalfSize().x;
	float centroZTapa = aab.getHalfSize().z;


	nodoCuerpo -> translate(0.0,alturaBase,0.0);
	nodoTapa -> translate(0.0,alturaCuerpo,0.0);
	
	Quaternion rotB(Degree(90),Vector3::UNIT_X);
	nodoBarril -> rotate(rotB);
	nodoBarril -> translate(0.0,alturaTapa,centroZTapa*2);
	alturaBarril = alturaTapa*15.0f;
	nodoBarril -> scale(0.5,1.5,0.5);
	

	nodo -> translate(vec);
	nodo -> scale (15.0,15.0,15.0);

	return nodo;
}

inline SceneNode *crearHelice(SceneManager *mSceneMgr, Vector3 vec) {
	SceneNode *nodoRaiz = mSceneMgr->createSceneNode();

	Entity *ent;
	SceneNode *nodo;

	nodo = mSceneMgr->createSceneNode();
	ent = mSceneMgr->createEntity("usb_laser.mesh");
	ent->setMaterialName("Examples/GrassFloor");
	nodo->attachObject(ent);
	nodoRaiz->addChild(nodo);

	nodo = mSceneMgr->createSceneNode();
	ent = mSceneMgr->createEntity("usb_laser.mesh");
	ent->setMaterialName("Examples/GrassFloor");
	nodo->attachObject(ent);
	nodo->rotate(Quaternion(Degree(180.0), Vector3::UNIT_Z));
	nodoRaiz->addChild(nodo);

	nodo = mSceneMgr->createSceneNode();
	ent = mSceneMgr->createEntity("usb_laser.mesh");
	ent->setMaterialName("Examples/GrassFloor");
	nodo->attachObject(ent);
	nodo->rotate(Quaternion(Degree(270.0), Vector3::UNIT_Z));
	nodoRaiz->addChild(nodo);

	nodo = mSceneMgr->createSceneNode();
	ent = mSceneMgr->createEntity("usb_laser.mesh");
	ent->setMaterialName("Examples/GrassFloor");
	nodo->attachObject(ent);
	nodo->rotate(Quaternion(Degree(90.0), Vector3::UNIT_Z));
	nodoRaiz->addChild(nodo);

	nodoRaiz->scale(400, 400, 400);
	nodoRaiz->translate(vec);
	return nodoRaiz;
}

class Example25FrameListener : public FrameListener {
private:
	SceneNode *_playerNode;
	SceneNode *_padreNode;
	Camera *_cam;
	OIS::InputManager *_man;
	OIS::Keyboard *_key;
	OIS::Mouse *_mouse;


public:

	Example25FrameListener(SceneNode *playerNode, SceneNode *padre, RenderWindow *win, Camera *cam) {
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

	bool frameStarted(const FrameEvent &evt) {
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
		_key->capture();
		_mouse->capture();
		_playerNode->_updateBounds();

		if (_key->isKeyDown(OIS::KC_ESCAPE))
			return false;

		if (_key->isKeyDown(OIS::KC_1))
			std::cout << _padreNode->getPosition() << "\n";

		Quaternion q;
		//Player Controls

		// Move player UP
		if (_key->isKeyDown(OIS::KC_UP))
			translatePlayer += Vector3(0.0, 1.0, 0.0);

		// Move player DOWN
		if (_key->isKeyDown(OIS::KC_DOWN))
			translatePlayer += Vector3(0.0, -1.0, 0.0);

		// Rotate player LEFT
		if (_key->isKeyDown(OIS::KC_LEFT))
			rotatePlayer = rotatePlayer * Quaternion(Degree((rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Y);

		// Rotate player RIGHT
		if (_key->isKeyDown(OIS::KC_RIGHT))
			rotatePlayer = rotatePlayer * Quaternion(Degree(-(rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Y);

		q = _playerNode->_getDerivedOrientation();
		Radian angulo = q.getYaw();

		// Move foward
		if (_key->isKeyDown(OIS::KC_W))
			translatePlayer += Vector3(-sin(angulo.valueRadians()), 0.0, -cos(angulo.valueRadians()));

		// Strafe Left
		if (_key->isKeyDown(OIS::KC_A))
			translatePlayer += Vector3(-cos(angulo.valueRadians()), 0.0, sin(angulo.valueRadians()));

		// Strafe Right
		if (_key->isKeyDown(OIS::KC_D))
			translatePlayer += Vector3(cos(angulo.valueRadians()), 0.0, -sin(angulo.valueRadians()));

		bool keyA = _key->isKeyDown(OIS::KC_A);
		bool keyD = _key->isKeyDown(OIS::KC_D);
		int sign;

		// Strafe left
		if (keyA && !keyD) {
			if (_playerNode->getOrientation().getRoll().valueDegrees() < 45.f)
				strafePlayer = strafePlayer * Quaternion(Degree((rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Z);

		} else if (keyD && !keyA) {
			if (_playerNode->getOrientation().getRoll().valueDegrees() > -45.f)
				strafePlayer = strafePlayer * Quaternion(Degree(-(rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Z);

		} else {
			if (_playerNode->getOrientation().getRoll().valueDegrees() > 0.1f)
				sign = -1;
			else if (_playerNode->getOrientation().getRoll().valueDegrees() < -0.1f)
				sign = 1;
			else
				sign = 0;

			strafePlayer = strafePlayer * Quaternion(Degree(sign * (rotationAmount * rotationSpeed * evt.timeSinceLastFrame)), Vector3::UNIT_Z);
		}

		Vector3 top = _playerNode->_getWorldAABB().getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_TOP);
		Vector3 bot = _playerNode->_getWorldAABB().getCorner(AxisAlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM);
		Vector3 leftFar = top;
		Vector3 rightNear = bot;

		Vector3 oldPosition = _padreNode->getPosition();
		Vector3 newPosition = translatePlayer * evt.timeSinceLastFrame * playerSpeed + _padreNode->getPosition();
		Real deltaY = newPosition.y - oldPosition.y;
		Real deltaX = newPosition.x - oldPosition.x;
		Real deltaZ = newPosition.z - oldPosition.z;
		Real newTop = top.y + deltaY;
		Real newBottom = bot.y + deltaY;
		Real newLeft = leftFar.x + deltaX;
		Real newRight = rightNear.x + deltaX;
		Real newFar = leftFar.z + deltaZ;
		Real newNear = rightNear.z + deltaZ;

		unsigned int pasillo;

		if (deltaY > 0.0f && newTop > bordes[0])
			translatePlayer.y = 0.0;

		if (deltaY < 0.0f && newBottom < bordes[1])
			translatePlayer.y = 0.0;

		if (newPosition.z < -19080)
			pasillo = 1;
		else
			pasillo = 0;

		//std::cout << pasillo << std::endl;

		if (deltaX > 0.0f && newRight > minMaxX[pasillo][1]) {
			translatePlayer.x = 0.0;
		}

		if (deltaX < 0.0f && newLeft < minMaxX[pasillo][0]) {
			translatePlayer.x = 0.0;
		}

		if (deltaZ > 0.0f && newNear > minMaxZ[pasillo][0]) {
			translatePlayer.z = 0.0;
		}

		if (deltaZ < 0.0f && newFar < minMaxZ[pasillo][1]) {
			translatePlayer.z = 0.0;
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
	SceneNode *player;
	SceneNode *padre;
	SceneNode *cameraNode;

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
		mainSceneMgr = mRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");
		mSceneMgr = mainSceneMgr;

		if (mOverlaySystem)
			mSceneMgr->addRenderQueueListener(mOverlaySystem);
	}

	void createScene() {
		cameraNode = mSceneMgr->createSceneNode("CameraNodo");
		cameraNode->attachObject(mCamera);
		mSceneMgr->setAmbientLight(ColourValue(1.0, 1.0, 1.0));
		mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
		Entity *ent01 = mSceneMgr->createEntity("MyEntity1", "ejes01.mesh");
		SceneNode *node01 = mSceneMgr->createSceneNode("Node01");
		mSceneMgr->getRootSceneNode()->addChild(node01);
		node01->attachObject(ent01);
		Entity *entEscenario01 = mSceneMgr->createEntity("EscenarioBase01", "proyectoOgreI.mesh");
		SceneNode *nodeEscenario01 = mSceneMgr->createSceneNode("NodeMesh01");
		mSceneMgr->getRootSceneNode()->addChild(nodeEscenario01);
		nodeEscenario01->attachObject(entEscenario01);


		//ManualObject 

		ManualObject* ala = mSceneMgr -> createManualObject("manual");
		ala -> begin("BaseWhiteNoLighting", RenderOperation::OT_POINT_LIST);


		ala-> end();

		SceneNode* nodeAla = mSceneMgr -> createSceneNode();
		nodeAla -> attachObject(ala);
		mSceneMgr -> getRootSceneNode() -> addChild(nodeAla);

		//Luces
		Light* light1 = mSceneMgr -> createLight("Light1");
		light1->setType(Light::LT_POINT);
		light1 -> setPosition(0,0,200);
		light1-> setDiffuseColour(0.8f,0.8f,0.8f);

		Entity *torus = mSceneMgr->createEntity("ObjetoPrueba", "usb_torus.mesh");
		Entity *cilindro =  mSceneMgr->createEntity("ObjetoPrueba1", "usb_cubomod01.mesh");
		torus -> setMaterialName("lambert3");
		player = mSceneMgr -> createSceneNode("player");
		player -> showBoundingBox(true);

		player -> attachObject(torus);
		player -> attachObject(cilindro);

		player -> scale(3.0,3.0,3.0);

		//Ogre::SceneNode* test = crearTorreta(mSceneMgr,Vector3(0.0,0.0,0.0));
		//mSceneMgr -> getRootSceneNode() -> addChild(test);

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
		helice->rotate(Quaternion(Degree(90), Vector3::UNIT_Y));
		helices[0] = helice;
		
		mSceneMgr->getRootSceneNode()->addChild(helice);
		helice = crearHelice(mSceneMgr, Vector3(helicesLoc[1][0], helicesLoc[1][1], helicesLoc[1][2]));
		helice->rotate(Quaternion(Degree(90), Vector3::UNIT_Y));
		helices[1] = helice;
		
		mSceneMgr->getRootSceneNode()->addChild(helice);
		mSceneMgr->getRootSceneNode()->addChild(padre);
	}

};


int main (void) {
	Example1 app;
	app.go();
	return 0;
}