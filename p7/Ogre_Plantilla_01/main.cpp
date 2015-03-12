#include "stdafx.h"
#include "player.h"
#include "coin.h"
#include "ring.h"
#include "obstacle.h"

std::vector<Ring> rings = std::vector<Ring>();
std::vector<Coin> coins = std::vector<Coin>();
std::vector<Obstacle> obstacles = std::vector<Obstacle>();

class MainLoopFL : public FrameListener {
	Player *player;
public:

	MainLoopFL(Player *p);
	bool frameStarted(const FrameEvent &evt);
};

class Starfox : public ExampleApplication {

public:
	Player *cPlayer;
	MainLoopFL *mainLoop;

	Starfox() {};
	~Starfox();

	void createFrameListener();
	void createCamera();
	void createScene();
	void chooseSceneManager();
	void resetGame(Player *player);
};

Starfox app;

MainLoopFL::MainLoopFL(Player *p) {
	player = p;
}

bool MainLoopFL:: frameStarted(const FrameEvent &evt) {
	Vector3 playerPos = player->_padreNode->getPosition();
	AxisAlignedBox bbox = player->_padreNode->_getWorldAABB();
	Real dtime = evt.timeSinceLastFrame;

	for (auto it = rings.begin(); it != rings.end(); ++it) {
		player->checkCollision(*it);
		it->onUpdate(dtime);
	}

	for (auto it = coins.begin(); it != coins.end(); ++it) {
		player->checkCollision(*it);
		it->onUpdate(dtime);
	}

	for (auto it = obstacles.begin(); it != obstacles.end(); ++it) {
		player->checkCollision(*it);
		it->onUpdate(dtime);
	}

	if (player->health <= 0) {
		player->reset();
		app.resetGame(player);
	}

	return player->onUpdate(dtime);
}

SceneNode* construirTorre(SceneManager* sceneMgr) {
	SceneNode* scnTorre = sceneMgr -> createSceneNode();
	Entity* entTorreBase = sceneMgr -> createEntity("poly02.mesh");
	entTorreBase -> setMaterialName("RustyBarrel");
	SceneNode* scnTorreBase = sceneMgr->createSceneNode();
	scnTorreBase -> attachObject(entTorreBase);
	scnTorreBase -> scale(1.5, 6.0, 1.5);
	scnTorreBase -> translate(0.0, 600.0, 0.0);
	scnTorre -> addChild(scnTorreBase);
	//
	scnTorreBase -> showBoundingBox(true);

	//
	for (int i = 0; i < 13 ; ++i) {
		Entity* entTorreAnillo = sceneMgr -> createEntity("poly04.mesh");
		entTorreAnillo -> setMaterialName("Examples/BumpyMetal");
		SceneNode* scnTorreAnillo = sceneMgr -> createSceneNode();
		scnTorreAnillo -> attachObject(entTorreAnillo);
		scnTorreAnillo -> translate(0.0, i * 100.0f, 0.0);
		scnTorre -> addChild(scnTorreAnillo);
	}

	Entity* entTorreTope = sceneMgr -> createEntity("poly16.mesh");
	entTorreTope -> setMaterialName("Examples/BumpyMetal");
	SceneNode* scnTorreTope = sceneMgr -> createSceneNode();
	scnTorreTope -> attachObject(entTorreTope);
	scnTorreTope -> translate(0.0, 1200.0, 0.0);
	scnTorreTope -> scale(1.5, 1.5, 1.5);
	scnTorreTope -> showBoundingBox(true);
	scnTorre -> addChild(scnTorreTope);
	return scnTorre;
}

SceneNode* construirRejas(SceneManager* sceneMgr, Vector3 pos) {
	float altura = 600.0;
	SceneNode* nodoTorre1 = construirTorre(sceneMgr);
	nodoTorre1 -> translate(600.0, 0.0, 0.0);
	SceneNode* nodoTorre2 = construirTorre(sceneMgr);
	nodoTorre2 -> translate(-600.0, 0.0, 0.0);
	SceneNode* nodoReja = sceneMgr -> createSceneNode();
	SceneNode* barras = sceneMgr -> createSceneNode();
	Quaternion rotacionBarra(Degree(90.0), Vector3::UNIT_Y);

	for (int i = 0 ; i < 6; ++i) {
		SceneNode* nodoBarra = sceneMgr -> createSceneNode();
		Entity* barra = sceneMgr -> createEntity("Poly02.mesh");
		barra -> setMaterialName("Examples/BumpyMetal");
		nodoBarra -> attachObject(barra);
		nodoBarra -> translate(0.0, 50.0f + i * 200.f, 0.0);
		nodoBarra -> rotate(rotacionBarra);
		nodoBarra -> scale(0.2, 0.1, 12.0);
		nodoBarra -> showBoundingBox(true);
		barras -> addChild(nodoBarra);
	}

	nodoReja -> addChild(nodoTorre1);
	nodoReja -> addChild(nodoTorre2);
	nodoReja -> addChild(barras);
	nodoReja -> translate(pos);
	return nodoReja;
}

Obstacle construirPilar(SceneManager* sceneMgr, Vector3 pos) {
	SceneNode* nodoPilar = sceneMgr -> createSceneNode();
	AxisAlignedBox AABPilar;
	Vector3 AABPilarMax;
	Vector3 AABPilarMin;
	int secciones = 3;
	//
	Entity* rect = sceneMgr -> createEntity("Poly02.mesh");
	rect -> setMaterialName("Examples/Rocky");
	Entity* spiral = sceneMgr -> createEntity("Poly09.mesh");
	spiral -> setMaterialName("Examples/Rocky");
	SceneNode* nodoRect = sceneMgr -> createSceneNode("Rect");
	SceneNode* nodoSpiral = sceneMgr -> createSceneNode("Spiral");
	nodoSpiral -> rotate(Quaternion(Degree(45.0), Vector3::UNIT_Y));
	nodoRect -> translate(0.0, 0.0, 0.0);
	nodoSpiral -> translate(0.0, 200, 0.0);
	nodoRect -> attachObject(rect);
	nodoRect -> _updateBounds();
	nodoSpiral->attachObject(spiral);
	AABPilarMin = nodoRect -> _getWorldAABB().getMinimum();
	AABPilarMax = nodoRect -> _getWorldAABB().getMaximum();
	nodoPilar -> addChild(nodoRect);
	nodoPilar -> addChild(nodoSpiral);

	//
	for (int i = 1; i < secciones; ++i) {
		Entity* rect = sceneMgr -> createEntity("Poly02.mesh");
		rect -> setMaterialName("Examples/Rocky");
		Entity* spiral = sceneMgr -> createEntity("Poly09.mesh");
		spiral -> setMaterialName("Examples/Rocky");
		SceneNode* nodoRect = sceneMgr -> createSceneNode("Rect" + i);
		SceneNode* nodoSpiral = sceneMgr -> createSceneNode("Spiral" + i);
		nodoSpiral -> rotate(Quaternion(Degree(45.0 + (45.0 * i)), Vector3::UNIT_Y));
		nodoRect -> rotate(Quaternion(Degree(45.0 * i), Vector3::UNIT_Y));
		nodoRect -> attachObject(rect);
		nodoSpiral->attachObject(spiral);
		nodoRect -> translate(0.0, i * 400.0, 0.0);
		nodoSpiral -> translate(0.0, 200 + (i * 400.0), 0.0);
		nodoRect -> _updateBounds();
		nodoSpiral -> _updateBounds();
		//nodoRect -> showBoundingBox(true);
		//nodoSpiral -> showBoundingBox(true);
		nodoPilar -> addChild(nodoRect);
		nodoPilar -> addChild(nodoSpiral);
	}

	AABPilarMax.y += (2 * (secciones) - 1) * 200.0;
	WireBoundingBox* lol = new WireBoundingBox();
	AABPilar.setExtents(AABPilarMin.x, AABPilarMin.y, AABPilarMin.z, AABPilarMax.x, AABPilarMax.y, AABPilarMax.z);
	lol -> setBoundingBox(AABPilar);
	lol -> setVisible(true);
	nodoPilar -> attachObject(lol);
	nodoPilar -> translate(pos);
	Obstacle pilar = Obstacle(nodoPilar, Vector3::ZERO, 50.0f, Vector3::UNIT_Y, 400.f);
	return pilar;
}

Obstacle construirSierra(SceneManager* sceneMgr, Vector3 pos) {
	Entity* entSierra = sceneMgr ->createEntity("saw2.mesh");
	entSierra->setMaterialName("Examples/SphereMappedRustySteel");
	SceneNode* nodoSierra = sceneMgr -> createSceneNode();
	nodoSierra->attachObject(entSierra);
	nodoSierra->translate(pos);
	Obstacle sierra = Obstacle(nodoSierra, Vector3::UNIT_X, 50.0f, Vector3::UNIT_X, 800.f);
	nodoSierra->rotate(Quaternion(Degree(90.f), Vector3::UNIT_Y));
	nodoSierra -> scale(120.0, 120.0, 420.0);
	return sierra;
}

Coin construirMoneda(SceneManager* sceneMgr, Vector3 pos) {
	Coin moneda;
	SceneNode* nodoMoneda = sceneMgr -> createSceneNode();
	Entity* entMoneda = sceneMgr -> createEntity("Poly14.mesh");
	entMoneda -> setMaterialName("coin");
	nodoMoneda -> attachObject(entMoneda);
	nodoMoneda -> rotate(Quaternion(Degree(90), Vector3::UNIT_X));
	nodoMoneda -> translate(pos);
	moneda = Coin(nodoMoneda);
	return moneda;
}

Ring construirAnillo(SceneManager* sceneMgr, Vector3 pos) {
	Ring anillo;
	SceneNode* nodoAnillo = sceneMgr -> createSceneNode();
	Entity* entAnillo = sceneMgr -> createEntity("Poly04.mesh");
	entAnillo -> setMaterialName("coin");
	nodoAnillo -> attachObject(entAnillo);
	nodoAnillo->scale(2.0f, 2.0f, 2.0f);
	nodoAnillo -> rotate(Quaternion(Degree(90), Vector3::UNIT_X));
	nodoAnillo -> translate(pos);
	anillo = Ring(nodoAnillo, sceneMgr);
	return anillo;
}

Starfox::~Starfox() {
	if (mainLoop)
		delete mainLoop;

	if (cPlayer)
		delete cPlayer;
}

void Starfox::createFrameListener() {
	mainLoop = new MainLoopFL(cPlayer);
	mRoot->addFrameListener(mainLoop);
}

void Starfox::createCamera() {
	mCamera = mSceneMgr->createCamera("MyCamera1");
	mCamera->setPosition(0, 50, 300);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(5);
}


void Starfox::chooseSceneManager() {
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");

	if (mOverlaySystem)
		mSceneMgr->addRenderQueueListener(mOverlaySystem);
}

void Starfox::createScene() {
	SceneNode *padre;
	SceneNode *cameraNode;
	cameraNode = mSceneMgr->createSceneNode();
	cameraNode->attachObject(mCamera);
	mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
	//Luces
	Light *light1 = mSceneMgr->createLight("Light1");
	light1->setType(Light::LT_POINT);
	light1->setPosition(0, 0, 200);
	light1-> setDiffuseColour(0.8f, 0.8f, 0.8f);
	padre = mSceneMgr->createSceneNode();
	padre->addChild(cameraNode);
	mSceneMgr->setSkyBox(true, "Examples/StormySkyBox");
	cPlayer = new Player(mSceneMgr, mWindow, padre);
	mSceneMgr->getRootSceneNode()->addChild(padre);
	//Creacion de primer obstaculo
	SceneNode* reja = construirRejas(mSceneMgr, Vector3(0.0, y_border[1], 10000.0));
	mSceneMgr -> getRootSceneNode() -> addChild(reja);
	obstacles.push_back(Obstacle(reja, Vector3::ZERO, 0.0f));
	//Construccion de las sierras
	Obstacle sierraPrueba = construirSierra(mSceneMgr, Vector3(0.0, 0.0, 5000.0));
	mSceneMgr -> getRootSceneNode() -> addChild(sierraPrueba._node);
	obstacles.push_back(sierraPrueba);
	//
	//Construccion de tercer obstaculo
	Obstacle pilar = construirPilar(mSceneMgr, Vector3(0.0, 0.0, 0.0));
	mSceneMgr -> getRootSceneNode() -> addChild(pilar._node);
	obstacles.push_back(pilar);
	//
	//Construccion de monedas
	Coin moneda = construirMoneda(mSceneMgr, Vector3(0.0, 0.0, 1000.0));
	mSceneMgr -> getRootSceneNode() -> addChild(moneda._node);
	coins.push_back(moneda);
	//
	//Construccion de anillos
	Ring anillo = construirAnillo(mSceneMgr, Vector3(0.0, 0.0, 500.0));
	mSceneMgr -> getRootSceneNode() -> addChild(anillo._node);
	rings.push_back(anillo);
	//
	Plane plane(Vector3::UNIT_Y, 0.0);
	MeshManager::getSingleton().createPlane("plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
											plane, 20000, 90000, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);
	Ogre::Entity* entPlano = mSceneMgr->createEntity("PlanoEntity", "plane");
	SceneNode* nodoPlano = mSceneMgr -> createSceneNode();
	entPlano -> setMaterialName("Examples/GrassFloor");
	nodoPlano -> attachObject(entPlano);
	nodoPlano -> translate(0.0, -5000.0, 44900.0);
	mSceneMgr->getRootSceneNode()->addChild(nodoPlano);
}

void Starfox::resetGame(Player* player) {
	std::cout << "Resetting" << std::endl;
	for (auto it = coins.begin(); it != coins.end(); ++it)
	{
		it->active = true;
		it->_node->setVisible(true);
	}

	Entity *ring;
	for (auto it = rings.begin(); it != rings.end(); ++it)
	{
		it->active = true;
		ring = static_cast<Entity*>(it->_node->getAttachedObject(0));
		ring->setMaterialName("ring");
	}
}

int main (void) {
	app.go();
	return 0;
}