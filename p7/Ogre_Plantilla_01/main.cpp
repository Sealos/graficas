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


	//
	for (int i = 0; i < 13 ; ++i) {
		Entity* entTorreAnillo = sceneMgr -> createEntity("poly04.mesh");
		entTorreAnillo -> setMaterialName("Examples/BumpyMetal");
		SceneNode* scnTorreAnillo = sceneMgr -> createSceneNode();
		scnTorreAnillo -> attachObject(entTorreAnillo);
		scnTorreAnillo -> translate(0.0, i * 100.0f, 0.0);
		scnTorre -> addChild(scnTorreAnillo);
		obstacles.push_back(Obstacle(scnTorreBase,Vector3::ZERO,0.0));
	}

	Entity* entTorreTope = sceneMgr -> createEntity("poly16.mesh");
	entTorreTope -> setMaterialName("Examples/BumpyMetal");
	SceneNode* scnTorreTope = sceneMgr -> createSceneNode();
	scnTorreTope -> attachObject(entTorreTope);
	scnTorreTope -> translate(0.0, 1200.0, 0.0);
	scnTorreTope -> scale(1.5, 1.5, 1.5);

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

		barras -> addChild(nodoBarra);
		obstacles.push_back(Obstacle(nodoBarra,Vector3::ZERO,0.0));
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
	SceneNode* nodoRect = sceneMgr -> createSceneNode();
	SceneNode* nodoSpiral = sceneMgr -> createSceneNode();
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
		SceneNode* nodoRect = sceneMgr -> createSceneNode();
		SceneNode* nodoSpiral = sceneMgr -> createSceneNode();
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
	nodoPilar -> attachObject(lol);
	nodoPilar -> scale(3.0,3.0,3.0);
	nodoPilar -> translate(pos);
	Obstacle pilar = Obstacle(nodoPilar, Vector3::ZERO, 50.0f, Vector3::UNIT_Y, 800.f);
	return pilar;
}

Obstacle construirSierra(SceneManager* sceneMgr, Vector3 pos) {
	Entity* entSierra = sceneMgr ->createEntity("saw2.mesh");
	entSierra->setMaterialName("Examples/SphereMappedRustySteel");
	SceneNode* nodoSierra = sceneMgr -> createSceneNode();
	nodoSierra->attachObject(entSierra);
	nodoSierra->translate(pos);
	Obstacle sierra = Obstacle(nodoSierra, Vector3::ZERO, 50.0f, Vector3::UNIT_X, 1200.f);
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
	nodoAnillo->scale(2.f, 2.f, 2.f);
	nodoAnillo -> rotate(Quaternion(Degree(90), Vector3::UNIT_X));
	nodoAnillo -> translate(pos);
	nodoAnillo->_updateBounds();
	anillo = Ring(nodoAnillo, sceneMgr);
	return anillo;
}

SceneNode* construirRectangulo(SceneManager* sceneMgr, Vector3 pos, Quaternion rot, Vector3 size){
	SceneNode* nodoRect = sceneMgr -> createSceneNode();
	Entity* entRect = sceneMgr -> createEntity("poly02.mesh");
	entRect -> setMaterialName("Examples/Rocky");
	nodoRect -> attachObject(entRect);
	nodoRect -> rotate(rot);
	nodoRect -> scale(size);
	nodoRect ->translate(pos);

	return nodoRect;

}

SceneNode* construirMeta(SceneManager* sceneMgr, Vector3 pos){

	SceneNode* nodoMeta = sceneMgr -> createSceneNode();
	return nodoMeta;
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
	mCamera->setPosition(0, 100, 600);
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
	//mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
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
	Real profundidadMaxima = 80000.0;
	Real profundidadMinima = 10000.0;
	//Creacion decorativa
	//Cercas del mundo
	int numeroCercas = 15;
	int separadorCercas = (y_border[0] - y_border[1])/numeroCercas;
	for(int i = 0;i<numeroCercas;++i){
		SceneNode* cercaNear = construirRectangulo(mSceneMgr,Vector3(0.0,y_border[1]+(i*separadorCercas),z_border[1]),Quaternion(Degree(90.0),Vector3::UNIT_Y),Vector3(1.0,1.0,200.0));
		mSceneMgr -> getRootSceneNode() -> addChild(cercaNear);

		SceneNode* cercaFar = construirRectangulo(mSceneMgr,Vector3(0.0,y_border[1]+(i*separadorCercas),z_border[0]),Quaternion(Degree(90.0),Vector3::UNIT_Y),Vector3(1.0,1.0,200.0));
		mSceneMgr -> getRootSceneNode() -> addChild(cercaFar);

		SceneNode* cercaLeft = construirRectangulo(mSceneMgr,Vector3(x_border[0]+150,y_border[1]+(i*separadorCercas),z_border[0]/2),Quaternion(),Vector3(1.0,1.0,900.0));
		mSceneMgr -> getRootSceneNode() -> addChild(cercaLeft);

		SceneNode* cercaRight = construirRectangulo(mSceneMgr,Vector3(x_border[1]-150,y_border[1]+(i*separadorCercas),z_border[0]/2),Quaternion(),Vector3(1.0,1.0,900.0));
		mSceneMgr -> getRootSceneNode() -> addChild(cercaRight);
	}

	//
	//Creacion de primer obstaculo
	
	//Construccion de las sierras
	int numSierras = 3;
	Real separadorSierras = z_border[0]-profundidadMinima/numSierras;
	for(int i =0; i< numSierras; ++i){
		Obstacle sierraPrueba = construirSierra(mSceneMgr, Vector3(4000*i, 0.0, profundidadMinima+(i*20000)));
		mSceneMgr -> getRootSceneNode() -> addChild(sierraPrueba._node);
		obstacles.push_back(sierraPrueba);
	}
	
	//
	//Construccion de tercer obstaculo
	
	//
	//Construccion de monedas
	int numeroMonedas = 20;
	
	Real separador =(profundidadMaxima-profundidadMinima)/numeroMonedas;
	for(int i =0; i<numeroMonedas; ++i){
		Coin moneda = construirMoneda(mSceneMgr, Vector3(sin(i)*8000.0, cos(i)*3000, profundidadMinima+ i*separador));
		mSceneMgr -> getRootSceneNode() -> addChild(moneda._node);
		coins.push_back(moneda);
		Obstacle pilar = construirPilar(mSceneMgr, Vector3(sin(i)*8000.0, cos(i)*3000, profundidadMinima+ i*separador));
	mSceneMgr -> getRootSceneNode() -> addChild(pilar._node);
	obstacles.push_back(pilar);
	}
	
	//
	//Construccion de anillos
	int numeroAnillos = 8;
	separador = (profundidadMaxima-profundidadMinima)/numeroAnillos;
	for(int i = 0; i< numeroAnillos; ++i){
		Ring anillo = construirAnillo(mSceneMgr, Vector3(cos(i)*8000.0, sin(i)*3000,  profundidadMinima + i*separador));
		mSceneMgr -> getRootSceneNode() -> addChild(anillo._node);
		rings.push_back(anillo);

		SceneNode* reja = construirRejas(mSceneMgr, Vector3(cos(i)*8000.0, sin(i)*3000,  (profundidadMinima + i*separador)));
		reja -> translate(0.0,-600.0,-200.0);
		mSceneMgr -> getRootSceneNode() -> addChild(reja);

		reja = construirRejas(mSceneMgr, Vector3(cos(i)*8000.0, sin(i)*3000,  (profundidadMinima + i*separador)));
		reja -> translate(0.0,-600.0,200.0);
		mSceneMgr -> getRootSceneNode() -> addChild(reja);
	}
	
	//
	Plane plane(Vector3::UNIT_Y, 0.0);
	MeshManager::getSingleton().createPlane("plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
											plane, 20000, 90000, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);
	Ogre::Entity* entPlano = mSceneMgr->createEntity("PlanoEntity", "plane");
	SceneNode* nodoPlano = mSceneMgr -> createSceneNode();
	entPlano -> setMaterialName("Examples/GrassFloor");
	nodoPlano -> attachObject(entPlano);
	nodoPlano -> translate(0.0, -5000.0, 45000.0);
	mSceneMgr->getRootSceneNode()->addChild(nodoPlano);
}

void Starfox::resetGame(Player* player) {
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
		ring->setMaterialName("coin");
	}
}

int main (void) {
	app.go();
	return 0;
}