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
	{0.f, -9280.f}, {0.f, -21000.f}
};


// width, height
float medidasPasillos[2][2] = {
	{3500.f, 19000.f}, {52000.f, 5000.f}
};

float minMaxX[2][2] = {
	{centroPasillos[0][0] - medidasPasillos[0][0] / 2, centroPasillos[0][0] + medidasPasillos[0][0] / 2},

	{centroPasillos[1][0] - medidasPasillos[1][0] / 2, centroPasillos[1][0] + medidasPasillos[1][0] / 2},
};

float minMaxZ[2][2] = {
	{centroPasillos[0][1] + medidasPasillos[0][1] / 2, centroPasillos[0][1] - medidasPasillos[0][1] / 2},

	{centroPasillos[1][1] + medidasPasillos[1][1] / 2, centroPasillos[1][1] - medidasPasillos[1][1] / 2},
};

Real ultimaFila = -23354;
Real penultimaFila = -18900;
Real altura = -332;

Vector3 posicionesT[8] = {
	Vector3(1615, altura, -15240),
	Vector3(8135, altura, ultimaFila),
	Vector3(24000, altura, ultimaFila),
	Vector3(-10000, altura, ultimaFila),
	Vector3(-23027, altura, ultimaFila),
	Vector3(-16939, altura, penultimaFila),
	Vector3(15400, altura, penultimaFila),
	Vector3(-1615, altura, -8573),
};

SceneManager *mainSceneMgr;

class Laser {
public:
	SceneNode *laserNodo;
	Entity *laserEnt;
	Vector3 direccionP;
	float velocidad;
	float aliveTime;

	Laser() {
		laserNodo = nullptr;
		laserEnt = nullptr;
		velocidad = 1.f;
		aliveTime = 0.f;
	}

	Laser(Vector3 vec, Vector3 playerDir) {
		laserEnt = mainSceneMgr -> createEntity("usb_laser.mesh");
		laserEnt->setMaterialName("lambert3");
		laserNodo = mainSceneMgr -> createSceneNode();
		laserNodo -> attachObject(laserEnt);
		Vector3 src = laserNodo -> getOrientation() * Vector3::NEGATIVE_UNIT_Y;
		Quaternion dir = src.getRotationTo(-playerDir);
		laserNodo -> rotate(dir);
		laserNodo -> translate(vec);
		laserNodo -> scale(4.0, 4.0, 4.0);
		direccionP = playerDir;
		velocidad = 25.f;
		aliveTime = 0.f;
	}

	void move(float timeDiff) {
		aliveTime += timeDiff;
		if (laserNodo)
			laserNodo -> translate(direccionP * velocidad);
	}

	~Laser() {
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

			Ogre::Vector3 direction = _playerNode->getPosition() - torretas[i]->getPosition();
			direction.y = 0.0;
			direction.normalise();

			Ogre::Vector3 vUp = Ogre::Vector3(0, 1.0f, 0.0f);
			Ogre::Vector3 vRight = vUp.crossProduct(direction);
			vUp = direction.crossProduct(vRight);

			Ogre::Matrix4 mBasis = Ogre::Matrix4(vRight.x,    vRight.y,    vRight.z,    0.0f,
												 vUp.x,       vUp.y,       vUp.z,       0.0f,
												 direction.x, direction.y, direction.z, 0.0f,
												 0.0f,        0.0f,        0.0f,        1.0f);

			Ogre::Quaternion mOrientDest = mBasis.extractQuaternion().Inverse();
			torretas[i]->setOrientation(mOrientDest);

			if (laserTimes[i].getMilliseconds() > tiempos[i]) {
				Vector3 playerDirection = (posicionesT[i] + Vector3(0.0, alturaBarril, 0.0)) - _playerNode -> getPosition();
				Real distance = playerDirection.normalise();
				laserTimes[i].reset();
				Laser *las = new Laser((posicionesT[i] + Vector3(0.0, alturaBarril, 0.0)), -playerDirection);
				mainSceneMgr -> getRootSceneNode() -> addChild(las->laserNodo);
				laseres.push_back(las);
			}
		}

		for (vector<Laser *>::iterator it = laseres.begin(); it != laseres.end();) {
			if ((*it)->aliveTime > 10){
				(*it)->laserNodo->setScale(0.0, 0.0, 0.0);
				it = laseres.erase(it);
			}
			else {
				(*it)-> move(evt.timeSinceLastFrame);
				++it;
			}
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

inline SceneNode *crearTorreta(SceneManager *mSceneMgr, Vector3 vec, int i) {

	//Cargar entidades
	Ogre::Entity *base = mSceneMgr->createEntity("usb_cubomod01.mesh");
	Ogre::Entity *cuerpo = mSceneMgr-> createEntity("usb_cilindro.mesh");
	Ogre::Entity *tapa = mSceneMgr-> createEntity("usb_formacurva.mesh");
	Ogre::Entity *barril = mSceneMgr -> createEntity("usb_cilindro02.mesh");

	//Asignar Materiales
	if (i > 4) {
		base -> setMaterialName("Examples/BeachStones");
		MaterialPtr mat = base->getSubEntity(0)->getMaterial();
		mat->setAmbient(0.1, 0.1, 0.1);
		mat->setDiffuse(Ogre::ColourValue(0.1, 0.1, 0.1, 0.2));
		mat->setShininess(0.1);
		mat->setSpecular(Ogre::ColourValue(0.1, 0.1, 0.1, 0.2));

		base->setMaterial(mat);
		cuerpo->setMaterial(mat);
		tapa->setMaterial(mat);
		barril->setMaterial(mat);
	} else {
		base -> setMaterialName("Examples/Chrome");
		cuerpo -> setMaterialName("Examples/Chrome");
		tapa -> setMaterialName("Examples/Chrome");
		barril -> setMaterialName("Examples/Chrome");
	}

	//Crear escenas
	Ogre::SceneNode *nodoBase = mSceneMgr->createSceneNode();
	Ogre::SceneNode *nodoCuerpo = mSceneMgr->createSceneNode();
	Ogre::SceneNode *nodoTapa = mSceneMgr->createSceneNode();
	SceneNode *nodoBarril = mSceneMgr-> createSceneNode();
	Ogre::SceneNode *nodo = mSceneMgr->createSceneNode();

	nodoBase -> attachObject(base);
	nodoCuerpo -> attachObject(cuerpo);
	nodoTapa -> attachObject(tapa);
	nodoBarril -> attachObject(barril);

	nodo -> addChild(nodoBase);
	nodo -> addChild(nodoCuerpo);
	nodo -> addChild(nodoTapa);
	nodo -> addChild(nodoBarril);

	nodoBase -> scale(3.0, 2.0, 3.0);
	nodoCuerpo -> scale(2.0, 2.0, 2.0);
	nodoBase -> _updateBounds();
	nodoCuerpo -> _updateBounds();
	nodoTapa -> _updateBounds();
	nodoBarril ->_updateBounds();

	Ogre::AxisAlignedBox aab = nodoBase -> _getWorldAABB();
	float alturaBase = aab.getSize().y;
	aab = nodoCuerpo -> _getWorldAABB();
	float alturaCuerpo = aab.getSize().y;
	aab = nodoCuerpo -> _getWorldAABB();
	float alturaTapa = aab.getSize().y;
	float centroXTapa = aab.getHalfSize().x;
	float centroZTapa = aab.getHalfSize().z;


	nodoCuerpo -> translate(0.0, alturaBase, 0.0);
	nodoTapa -> translate(0.0, alturaCuerpo, 0.0);

	Quaternion rotB(Degree(90), Vector3::UNIT_X);
	nodoBarril -> rotate(rotB);
	nodoBarril -> translate(0.0, alturaTapa, centroZTapa * 2);
	alturaBarril = alturaTapa * 15.0f;
	nodoBarril -> scale(0.5, 1.5, 0.5);


	nodo -> translate(vec);
	nodo -> scale (15.0, 15.0, 15.0);

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

	MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
	mat->setAmbient(0.1, 0.1, 0.1);
	mat->setDiffuse(Ogre::ColourValue(0.1, 0.1, 0.1, 0.2));
	mat->setShininess(0.1);
	mat->setSpecular(Ogre::ColourValue(0.1, 0.1, 0.1, 0.2));

	nodoRaiz->scale(350, 350, 350);
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
		//_playerNode->_updateBounds();

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
			if (_playerNode->getOrientation().getRoll().valueDegrees() > 0.2f)
				sign = -1;
			else if (_playerNode->getOrientation().getRoll().valueDegrees() < -0.2f)
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

		if (newPosition.z > -18700 && newPosition.x > (minMaxX[0][0] - 50.f) && newPosition.x < (minMaxX[0][1] + 50.f))
			pasillo = 0;
		else
			pasillo = 1;

		//std::cout << pasillo << std::endl;

		if (deltaX > 0.0f && (newRight > minMaxX[pasillo][1])) {
			translatePlayer.x = 0.0;
		}

		if (deltaX < 0.0f && (newLeft < minMaxX[pasillo][0])) {
			translatePlayer.x = 0.0;
		}

		if (deltaZ > 0.0f && (newNear > minMaxZ[pasillo][0])) {
				translatePlayer.z = 0.0;
		}

		if (deltaZ < 0.0f && (newFar < minMaxZ[pasillo][1])) {
			translatePlayer.z = 0.0;
		}

		_padreNode->translate(translatePlayer * evt.timeSinceLastFrame * playerSpeed);
		_padreNode->rotate(rotatePlayer);
		_playerNode->rotate(strafePlayer);
		return true;
	}
};

Vector3 calcularNormal(Vector3 a, Vector3 b, Vector3 c){
	return (b - a).crossProduct(c - a);
}

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
		mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));
		mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
		Entity *entEscenario01 = mSceneMgr->createEntity("EscenarioBase01", "proyectoOgreI.mesh");
		SceneNode *nodeEscenario01 = mSceneMgr->createSceneNode("NodeMesh01");
		mSceneMgr->getRootSceneNode()->addChild(nodeEscenario01);
		nodeEscenario01->attachObject(entEscenario01);

		//Primer Cuadrado {0,3)
float primerCuad = 0.5;
float segundoCuad = 1.0;
float interiorAlaX = 3.5;
float puntaAlaZ = 6.0;
float grosor = 0.5;
//Primer Cuadrado {0,3)
double vertices[][3] = {
	{0.0, primerCuad, primerCuad},
	{0.0, primerCuad, -primerCuad},
	{0.0, -primerCuad, -primerCuad},
	{0.0, -primerCuad, primerCuad},

	//Segundo cuadrado {4,7)

	{interiorAlaX, segundoCuad, segundoCuad},
	{interiorAlaX, segundoCuad, -segundoCuad},
	{interiorAlaX, -segundoCuad, -segundoCuad},
	{interiorAlaX, -segundoCuad, segundoCuad},

	//Bordes del Interior de la ala {8,15)
	{interiorAlaX - (interiorAlaX / 4), 2 * segundoCuad, segundoCuad},
	{interiorAlaX, segundoCuad, 2 * segundoCuad},
	{interiorAlaX, -segundoCuad, 2 * segundoCuad},
	{interiorAlaX - (interiorAlaX / 4), 2 * -segundoCuad, segundoCuad},

	{interiorAlaX - (interiorAlaX / 4), 2 * -segundoCuad, -segundoCuad},
	{interiorAlaX - (interiorAlaX / 4), 2 * segundoCuad, -segundoCuad},

	
	{interiorAlaX, segundoCuad, -segundoCuad - puntaAlaZ},
	{interiorAlaX, -segundoCuad, -segundoCuad - puntaAlaZ},

	//Grosor de alas
	

	//Bordes del Exterior de la ala {16,23)
	{interiorAlaX - (interiorAlaX / 4) + grosor, 2 * segundoCuad, segundoCuad},
	{interiorAlaX + grosor, segundoCuad, 2 * segundoCuad},
	{interiorAlaX + grosor, -segundoCuad, 2 * segundoCuad},
	{interiorAlaX - (interiorAlaX / 4) + grosor, 2 * -segundoCuad, segundoCuad},

	{interiorAlaX - (interiorAlaX / 4) + grosor, 2 * -segundoCuad, -segundoCuad},
	{interiorAlaX - (interiorAlaX / 4) + grosor, 2 * segundoCuad, -segundoCuad},

	{interiorAlaX + grosor, segundoCuad, -segundoCuad - puntaAlaZ},
	{interiorAlaX + grosor, -segundoCuad, -segundoCuad - puntaAlaZ},


	//Segundo cuadrado grosor {24,27)
	{interiorAlaX + grosor, segundoCuad, segundoCuad},
	{interiorAlaX + grosor, segundoCuad, -segundoCuad},
	{interiorAlaX + grosor, -segundoCuad, -segundoCuad},
	{interiorAlaX + grosor, -segundoCuad, segundoCuad},
};

Vector3 verticesV[28];

for(int i =0; i < 28; ++i){
	verticesV[i] = Vector3(vertices[i][0],vertices[i][1],vertices[i][2]);
}
Vector3 normales[28];

//Calculando Normales
normales[0] = calcularNormal(verticesV[0],verticesV[1],verticesV[2])+
				calcularNormal(verticesV[0],verticesV[2],verticesV[3])+
				calcularNormal(verticesV[0],verticesV[4],verticesV[3])+
				calcularNormal(verticesV[0],verticesV[4],verticesV[1]);

normales[1] = calcularNormal(verticesV[1],verticesV[0],verticesV[2])+
				calcularNormal(verticesV[1],verticesV[4],verticesV[0])+
				calcularNormal(verticesV[1],verticesV[5],verticesV[4])+
				calcularNormal(verticesV[1],verticesV[5],verticesV[6])+
				calcularNormal(verticesV[1],verticesV[6],verticesV[2]);

normales[2] = calcularNormal(verticesV[2],verticesV[0],verticesV[1])+
				calcularNormal(verticesV[2],verticesV[3],verticesV[0])+
				calcularNormal(verticesV[2],verticesV[3],verticesV[6])+
				calcularNormal(verticesV[2],verticesV[1],verticesV[6]);

normales[3] = calcularNormal(verticesV[3],verticesV[2],verticesV[0])+
				calcularNormal(verticesV[3],verticesV[4],verticesV[0])+
				calcularNormal(verticesV[3],verticesV[7],verticesV[4])+
				calcularNormal(verticesV[3],verticesV[6],verticesV[7])+
				calcularNormal(verticesV[3],verticesV[2],verticesV[6]);

normales[4] = calcularNormal(verticesV[4],verticesV[5],verticesV[6])+
				calcularNormal(verticesV[4],verticesV[6],verticesV[7])+
				calcularNormal(verticesV[4],verticesV[0],verticesV[3])+
				calcularNormal(verticesV[4],verticesV[7],verticesV[3])+
				calcularNormal(verticesV[4],verticesV[1],verticesV[0])+
				calcularNormal(verticesV[4],verticesV[5],verticesV[1])+
				calcularNormal(verticesV[4],verticesV[9],verticesV[8])+
				calcularNormal(verticesV[4],verticesV[9],verticesV[7])+
				calcularNormal(verticesV[4],verticesV[5],verticesV[8]);

normales[5] = calcularNormal(verticesV[5],verticesV[4],verticesV[6])+
				calcularNormal(verticesV[5],verticesV[1],verticesV[4])+
				calcularNormal(verticesV[5],verticesV[1],verticesV[6])+
				calcularNormal(verticesV[5],verticesV[13],verticesV[14])+
				calcularNormal(verticesV[5],verticesV[8],verticesV[13])+
				calcularNormal(verticesV[5],verticesV[4],verticesV[8])+
				calcularNormal(verticesV[5],verticesV[6],verticesV[26])+
				calcularNormal(verticesV[5],verticesV[26],verticesV[25])+
				calcularNormal(verticesV[5],verticesV[14],verticesV[25]);

normales[6] = calcularNormal(verticesV[6],verticesV[4],verticesV[5])+
				calcularNormal(verticesV[6],verticesV[7],verticesV[4])+
				calcularNormal(verticesV[6],verticesV[3],verticesV[7])+
				calcularNormal(verticesV[6],verticesV[3],verticesV[2])+
				calcularNormal(verticesV[6],verticesV[1],verticesV[5])+
				calcularNormal(verticesV[6],verticesV[1],verticesV[2])+
				calcularNormal(verticesV[6],verticesV[7],verticesV[11])+
				calcularNormal(verticesV[6],verticesV[11],verticesV[12])+
				calcularNormal(verticesV[6],verticesV[15],verticesV[12])+
				calcularNormal(verticesV[6],verticesV[15],verticesV[26])+
				calcularNormal(verticesV[6],verticesV[5],verticesV[26]);

normales[7] = calcularNormal(verticesV[7],verticesV[6],verticesV[4])+
				calcularNormal(verticesV[7],verticesV[4],verticesV[3])+
				calcularNormal(verticesV[7],verticesV[6],verticesV[3])+
				calcularNormal(verticesV[7],verticesV[9],verticesV[4])+
				calcularNormal(verticesV[7],verticesV[9],verticesV[10])+
				calcularNormal(verticesV[7],verticesV[10],verticesV[14])+
				calcularNormal(verticesV[7],verticesV[6],verticesV[11]);

normales[8] = calcularNormal(verticesV[8],verticesV[9],verticesV[4])+
				calcularNormal(verticesV[8],verticesV[5],verticesV[13])+
				calcularNormal(verticesV[8],verticesV[5],verticesV[4])+
				calcularNormal(verticesV[8],verticesV[9],verticesV[16])+
				calcularNormal(verticesV[8],verticesV[13],verticesV[21])+
				calcularNormal(verticesV[8],verticesV[21],verticesV[16]);

normales[9] = calcularNormal(verticesV[9],verticesV[4],verticesV[8])+
				calcularNormal(verticesV[9],verticesV[4],verticesV[7])+
				calcularNormal(verticesV[9],verticesV[7],verticesV[10])+
				calcularNormal(verticesV[9],verticesV[8],verticesV[16])+
				calcularNormal(verticesV[9],verticesV[13],verticesV[21])+
				calcularNormal(verticesV[9],verticesV[21],verticesV[16]);

normales[10] = calcularNormal(verticesV[10],verticesV[9],verticesV[7])+
				calcularNormal(verticesV[10],verticesV[7],verticesV[11])+
				calcularNormal(verticesV[10],verticesV[9],verticesV[17])+
				calcularNormal(verticesV[10],verticesV[17],verticesV[18])+
				calcularNormal(verticesV[10],verticesV[11],verticesV[18]);

normales[11] = calcularNormal(verticesV[11],verticesV[7],verticesV[10])+
				calcularNormal(verticesV[11],verticesV[7],verticesV[6])+
				calcularNormal(verticesV[11],verticesV[6],verticesV[12])+
				calcularNormal(verticesV[11],verticesV[10],verticesV[18])+
				calcularNormal(verticesV[11],verticesV[18],verticesV[19])+
				calcularNormal(verticesV[11],verticesV[12],verticesV[19]);

normales[12] = calcularNormal(verticesV[12],verticesV[11],verticesV[6])+
				calcularNormal(verticesV[12],verticesV[6],verticesV[15])+
				calcularNormal(verticesV[12],verticesV[11],verticesV[19])+
				calcularNormal(verticesV[12],verticesV[19],verticesV[20])+
				calcularNormal(verticesV[12],verticesV[15],verticesV[20]);

normales[13] = calcularNormal(verticesV[13],verticesV[5],verticesV[14])+
				calcularNormal(verticesV[13],verticesV[5],verticesV[8])+
				calcularNormal(verticesV[13],verticesV[14],verticesV[21])+
				calcularNormal(verticesV[13],verticesV[8],verticesV[21]);

normales[14] = calcularNormal(verticesV[14],verticesV[5],verticesV[13])+
				calcularNormal(verticesV[14],verticesV[5],verticesV[25])+
				calcularNormal(verticesV[14],verticesV[25],verticesV[22])+
				calcularNormal(verticesV[14],verticesV[13],verticesV[21])+
				calcularNormal(verticesV[14],verticesV[21],verticesV[22]);

normales[15] = calcularNormal(verticesV[15],verticesV[6],verticesV[12])+
				calcularNormal(verticesV[15],verticesV[12],verticesV[20])+
				calcularNormal(verticesV[15],verticesV[20],verticesV[23])+
				calcularNormal(verticesV[15],verticesV[6],verticesV[26])+
				calcularNormal(verticesV[15],verticesV[26],verticesV[23]);

normales[16] = calcularNormal(verticesV[16],verticesV[9],verticesV[17])+
				calcularNormal(verticesV[16],verticesV[8],verticesV[9])+
				calcularNormal(verticesV[16],verticesV[21],verticesV[8])+
				calcularNormal(verticesV[16],verticesV[24],verticesV[17])+
				calcularNormal(verticesV[16],verticesV[24],verticesV[21]);

normales[17] = calcularNormal(verticesV[17],verticesV[16],verticesV[9])+
				calcularNormal(verticesV[17],verticesV[9],verticesV[10])+
				calcularNormal(verticesV[17],verticesV[10],verticesV[18])+
				calcularNormal(verticesV[17],verticesV[24],verticesV[16])+
				calcularNormal(verticesV[17],verticesV[24],verticesV[18]);

normales[18] = calcularNormal(verticesV[18],verticesV[17],verticesV[10])+
				calcularNormal(verticesV[18],verticesV[10],verticesV[11])+
				calcularNormal(verticesV[18],verticesV[11],verticesV[19])+
				calcularNormal(verticesV[18],verticesV[24],verticesV[17])+
				calcularNormal(verticesV[18],verticesV[27],verticesV[24])+
				calcularNormal(verticesV[18],verticesV[19],verticesV[27]);

normales[19] = calcularNormal(verticesV[19],verticesV[18],verticesV[11])+
				calcularNormal(verticesV[19],verticesV[11],verticesV[12])+
				calcularNormal(verticesV[19],verticesV[12],verticesV[20])+
				calcularNormal(verticesV[19],verticesV[18],verticesV[27])+
				calcularNormal(verticesV[19],verticesV[27],verticesV[20]);

normales[20] = calcularNormal(verticesV[20],verticesV[19],verticesV[12])+
				calcularNormal(verticesV[20],verticesV[12],verticesV[15])+
				calcularNormal(verticesV[20],verticesV[15],verticesV[23])+
				calcularNormal(verticesV[20],verticesV[27],verticesV[19])+
				calcularNormal(verticesV[20],verticesV[26],verticesV[27])+
				calcularNormal(verticesV[20],verticesV[23],verticesV[26]);

normales[21] = calcularNormal(verticesV[21],verticesV[14],verticesV[13])+
				calcularNormal(verticesV[21],verticesV[22],verticesV[14])+
				calcularNormal(verticesV[21],verticesV[13],verticesV[8])+
				calcularNormal(verticesV[21],verticesV[8],verticesV[16])+
				calcularNormal(verticesV[21],verticesV[25],verticesV[22])+
				calcularNormal(verticesV[21],verticesV[25],verticesV[24])+
				calcularNormal(verticesV[21],verticesV[24],verticesV[16]);

normales[22] = calcularNormal(verticesV[22],verticesV[25],verticesV[14])+
				calcularNormal(verticesV[22],verticesV[21],verticesV[14])+
				calcularNormal(verticesV[22],verticesV[25],verticesV[21]);

normales[23]= calcularNormal(verticesV[23],verticesV[20],verticesV[15])+
				calcularNormal(verticesV[23],verticesV[26],verticesV[15])+
				calcularNormal(verticesV[23],verticesV[20],verticesV[26]);

normales[24] = calcularNormal(verticesV[24],verticesV[16],verticesV[17])+
				calcularNormal(verticesV[24],verticesV[17],verticesV[18])+
				calcularNormal(verticesV[24],verticesV[18],verticesV[27])+
				calcularNormal(verticesV[24],verticesV[25],verticesV[21])+
				calcularNormal(verticesV[24],verticesV[21],verticesV[16])+
				calcularNormal(verticesV[24],verticesV[25],verticesV[26])+
				calcularNormal(verticesV[24],verticesV[27],verticesV[26]);

normales[25] = calcularNormal(verticesV[25],verticesV[26],verticesV[5])+
				calcularNormal(verticesV[25],verticesV[5],verticesV[14])+
				calcularNormal(verticesV[25],verticesV[14],verticesV[22])+
				calcularNormal(verticesV[25],verticesV[22],verticesV[21])+
				calcularNormal(verticesV[25],verticesV[21],verticesV[24])+
				calcularNormal(verticesV[25],verticesV[24],verticesV[26]);

normales[26] = calcularNormal(verticesV[26],verticesV[15],verticesV[6])+
				calcularNormal(verticesV[26],verticesV[23],verticesV[15])+
				calcularNormal(verticesV[26],verticesV[6],verticesV[5])+
				calcularNormal(verticesV[26],verticesV[5],verticesV[25])+
				calcularNormal(verticesV[26],verticesV[20],verticesV[27])+
				calcularNormal(verticesV[26],verticesV[20],verticesV[23])+
				calcularNormal(verticesV[26],verticesV[25],verticesV[24])+
				calcularNormal(verticesV[26],verticesV[27],verticesV[24]);

normales[27] = calcularNormal(verticesV[27],verticesV[18],verticesV[24])+
				calcularNormal(verticesV[27],verticesV[18],verticesV[19])+
				calcularNormal(verticesV[27],verticesV[19],verticesV[20])+
				calcularNormal(verticesV[27],verticesV[20],verticesV[26])+
				calcularNormal(verticesV[27],verticesV[26],verticesV[24]);

for(int i =0;i<28;++i){
	normales[i].normalise();
}



				ManualObject *ala = mSceneMgr -> createManualObject("manual");
		ala -> begin("lambert3", RenderOperation::OT_TRIANGLE_LIST);

		//Primer Cuadrado (0,3)
		ala-> position(0.0, primerCuad, primerCuad);
		ala -> normal(normales[0]);
		ala-> position(0.0, primerCuad, -primerCuad);
		ala -> normal(normales[1]);
		ala-> position(0.0, -primerCuad, -primerCuad);
		ala -> normal(normales[2]);
		ala-> position(0.0, -primerCuad, primerCuad);
		ala -> normal(normales[3]);

		//Segundo cuadrado (4,7)

		ala -> position(interiorAlaX, segundoCuad, segundoCuad);
		ala -> normal(normales[4]);
		ala -> position(interiorAlaX, segundoCuad, -segundoCuad);
		ala -> normal(normales[5]);
		ala -> position(interiorAlaX, -segundoCuad, -segundoCuad);
		ala -> normal(normales[6]);
		ala -> position(interiorAlaX, -segundoCuad, segundoCuad);
		ala -> normal(normales[7]);

		//Bordes del Interior de la ala (8,15)
		ala -> position(interiorAlaX - (interiorAlaX / 4), 2 * segundoCuad, segundoCuad);
		ala -> normal(normales[8]);
		ala -> position(interiorAlaX, segundoCuad, 2 * segundoCuad);
		ala -> normal(normales[9]);
		ala -> position(interiorAlaX, -segundoCuad, 2 * segundoCuad);
		ala -> normal(normales[10]);
		ala -> position(interiorAlaX - (interiorAlaX / 4), 2 * -segundoCuad, segundoCuad);
		ala -> normal(normales[11]);

		ala -> position(interiorAlaX - (interiorAlaX / 4), 2 * -segundoCuad, -segundoCuad);
		ala -> normal(normales[12]);
		ala -> position(interiorAlaX - (interiorAlaX / 4), 2 * segundoCuad, -segundoCuad);
		ala -> normal(normales[13]);


		ala -> position(interiorAlaX, segundoCuad, -segundoCuad - puntaAlaZ);
		ala -> normal(normales[14]);
		ala -> position(interiorAlaX, -segundoCuad, -segundoCuad - puntaAlaZ);
		ala -> normal(normales[15]);



		//Bordes del Exterior de la ala (16,23)
		ala -> position(interiorAlaX - (interiorAlaX / 4) + grosor, 2 * segundoCuad, segundoCuad);
		ala -> normal(normales[16]);
		ala -> position(interiorAlaX + grosor, segundoCuad, 2 * segundoCuad);
		ala -> normal(normales[17]);
		ala -> position(interiorAlaX + grosor, -segundoCuad, 2 * segundoCuad);
		ala -> normal(normales[18]);
		ala -> position(interiorAlaX - (interiorAlaX / 4) + grosor, 2 * -segundoCuad, segundoCuad);
		ala -> normal(normales[19]);

		ala -> position(interiorAlaX - (interiorAlaX / 4) + grosor, 2 * -segundoCuad, -segundoCuad);
		ala -> normal(normales[20]);
		ala -> position(interiorAlaX - (interiorAlaX / 4) + grosor, 2 * segundoCuad, -segundoCuad);
		ala -> normal(normales[21]);

		ala -> position(interiorAlaX + grosor, segundoCuad, -segundoCuad - puntaAlaZ);
		ala -> normal(normales[22]);
		ala -> position(interiorAlaX + grosor, -segundoCuad, -segundoCuad - puntaAlaZ);
		ala -> normal(normales[23]);


		//Segundo cuadrado grosor (24,27)
		ala -> position(interiorAlaX + grosor, segundoCuad, segundoCuad);
		ala -> normal(normales[24]);
		ala -> position(interiorAlaX + grosor, segundoCuad, -segundoCuad);
		ala -> normal(normales[25]);
		ala -> position(interiorAlaX + grosor, -segundoCuad, -segundoCuad);
		ala -> normal(normales[26]);
		ala -> position(interiorAlaX + grosor, -segundoCuad, segundoCuad);
		ala -> normal(normales[27]);
		//INDICES
		//Indices primer cuadrado
		ala -> index(0);
		ala -> index(1);
		ala -> index(2);

		ala -> index(2);
		ala -> index(3);
		ala ->index(0);
		//Indices segundo cuadrado
		ala -> index(4);
		ala -> index(5);
		ala -> index(6);

		ala -> index(6);
		ala -> index(7);
		ala -> index(4);

		//Indices que conectan estos cuadrados
		//Parte de atras
		ala -> index(4);
		ala -> index(0);
		ala -> index(3);

		ala -> index(7);
		ala -> index(4);
		ala -> index(3);
		//Parte de arriba
		ala -> index(4);
		ala -> index(1);
		ala -> index(0);

		ala -> index(5);
		ala -> index(1);
		ala -> index(4);
		//Parte de abajo
		ala -> index(3);
		ala -> index(6);
		ala -> index(7);

		ala -> index(3);
		ala -> index(2);
		ala -> index(6);

		//Parte de adelante
		ala -> index(1);
		ala -> index(5);
		ala -> index(6);

		ala -> index(1);
		ala -> index(6);
		ala -> index(2);

		//Indices Ala interna
		ala -> index(4);
		ala -> index(9);
		ala -> index(8);

		ala -> index(9);
		ala -> index(4);
		ala -> index(7);

		ala -> index(9);
		ala -> index(7);
		ala -> index(10);

		ala -> index(10);
		ala ->index(7);
		ala -> index(11);

		ala -> index(7);
		ala -> index(6);
		ala -> index(11);

		ala -> index(11);
		ala -> index(6);
		ala -> index(12);

		ala -> index(6);
		ala -> index(15);
		ala -> index(12);

		ala -> index(5);
		ala -> index(13);
		ala -> index(14);

		ala -> index(5);
		ala -> index(8);
		ala -> index(13);

		ala -> index(5);
		ala -> index(4);
		ala -> index(8);

		//Indices para el grosor de las alas
		ala -> index(8);
		ala -> index(9);
		ala -> index(16);

		ala -> index(16);
		ala -> index(9);
		ala -> index(17);

		ala -> index(9);
		ala -> index(10);
		ala -> index(17);

		ala -> index(17);
		ala -> index(10);
		ala -> index(18);

		ala -> index(10);
		ala -> index(11);
		ala -> index(18);

		ala -> index(18);
		ala -> index(11);
		ala -> index(19);

		ala -> index(11);
		ala -> index(12);
		ala -> index(19);

		ala -> index(19);
		ala -> index(12);
		ala -> index(20);

		ala -> index(12);
		ala -> index(15);
		ala -> index(20);

		ala -> index(20);
		ala -> index(15);
		ala -> index(23);

		ala -> index(15);
		ala -> index(6);
		ala -> index(26);

		ala -> index(26);
		ala -> index(23);
		ala -> index(15);

		ala -> index(6);
		ala -> index(5);
		ala -> index(26);

		ala -> index(26);
		ala -> index(5);
		ala -> index(25);

		ala -> index(5);
		ala -> index(14);
		ala -> index(25);

		ala -> index(25);
		ala -> index(14);
		ala -> index(22);

		ala -> index(14);
		ala -> index(13);
		ala -> index(21);

		ala -> index(21);
		ala -> index(22);
		ala -> index(14);

		ala -> index(13);
		ala -> index(8);
		ala -> index(21);

		ala -> index(21);
		ala -> index(8);
		ala -> index(16);

		//Indices de la ala exterior

		ala -> index(24);
		ala -> index(16);
		ala -> index(17);

		ala -> index(24);
		ala -> index(17);
		ala -> index(18);

		ala -> index(18);
		ala -> index(27);
		ala -> index(24);

		ala -> index(18);
		ala -> index(19);
		ala -> index(27);

		ala -> index(27);
		ala -> index(19);
		ala -> index(20);

		ala -> index(20);
		ala -> index(26);
		ala -> index(27);

		ala -> index(20);
		ala -> index(23);
		ala -> index(26);

		ala -> index(25);
		ala -> index(22);
		ala -> index(21);

		ala -> index(25);
		ala -> index(21);
		ala -> index(24);

		ala -> index(24);
		ala -> index(21);
		ala -> index(16);




		ala -> index(25);
		ala -> index(24);
		ala -> index(26);

		ala -> index(27);
		ala -> index(26);
		ala -> index(24);



		ala-> end();
		MeshPtr pMeshAla = ala -> convertToMesh("ala1");
		Entity *ala1 = mSceneMgr -> createEntity(pMeshAla);
		Entity *ala2 = mSceneMgr -> createEntity(pMeshAla);
		SceneNode *nodeAla = mSceneMgr -> createSceneNode();
		nodeAla -> attachObject(ala1);
		nodeAla -> translate(5.0, 0.0, 0.0);
		nodeAla -> scale(2.0, 2.0, 2.0);

		SceneNode *nodeAla2 = mSceneMgr -> createSceneNode();
		nodeAla2 -> attachObject(ala2);
		nodeAla2 -> translate(-5.0, 0.0, 0.0);

		nodeAla2 -> rotate(Quaternion(Degree(180), Vector3::UNIT_Z));
		nodeAla2 -> scale(2.0, 2.0, 2.0);


		//Luces
		Light *light1 = mSceneMgr -> createLight("Light1");
		light1->setType(Light::LT_POINT);
		light1 -> setPosition(0, 0, 200);
		light1-> setDiffuseColour(0.8f, 0.8f, 0.8f);

		Entity *torus = mSceneMgr->createEntity("ObjetoPrueba", "usb_torus.mesh");
		Entity *cilindro =  mSceneMgr->createEntity("ObjetoPrueba1", "usb_cubomod01.mesh");
		torus -> setMaterialName("lambert3");
		player = mSceneMgr -> createSceneNode("player");
		//player -> showBoundingBox(true);
		player -> addChild(nodeAla);
		player -> addChild(nodeAla2);
		player -> attachObject(torus);


		player -> scale(3.0, 3.0, 3.0);

		for (int i = 0 ; i < 8 ; ++i) {
			torretas[i] = crearTorreta(mSceneMgr, posicionesT[i], i);
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