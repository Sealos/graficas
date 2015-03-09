#include "stdafx.h"
#include "player.h"
#include "coin.h"
#include "ring.h"
#include "obstacle.h"
#include "sierra.h"
#include "pilar.h"

std::vector<Ring*> rings = std::vector<Ring*>(20);
std::vector<Coin*> coins = std::vector<Coin*>(20);
std::vector<Obstacle*> obstacles = std::vector<Obstacle*>(1);

class MainLoopFL : public FrameListener {
private:
	Player *player;
public:

	MainLoopFL(Player *p) {
		player = p;
	}

	~MainLoopFL() {
	}

	bool frameStarted(const FrameEvent &evt) {
		Vector3 playerPos = player->_padreNode->getPosition();
		AxisAlignedBox bbox = player->_padreNode->_getWorldAABB();
		Real dtime = evt.timeSinceLastFrame;

		for (auto it = rings.begin(); it != rings.end(); ++it) {
			player->checkCollision(*it);
			(*it)->onUpdate(dtime);
		}

		for (auto it = coins.begin(); it != coins.end(); ++it) {
			player->checkCollision(*it);
			(*it)->onUpdate(dtime);
		}

		for (auto it = obstacles.begin(); it != obstacles.end(); ++it) {
			player->checkCollision(*it);
			(*it)->onUpdate(dtime);
		}

		return player->onUpdate(dtime);
	}
};

SceneNode* construirTorre(SceneManager* sceneMgr){
	SceneNode* scnTorre = sceneMgr -> createSceneNode();
	

	Entity* entTorreBase = sceneMgr -> createEntity("poly02.mesh");
	entTorreBase -> setMaterialName("RustyBarrel");
	SceneNode* scnTorreBase = sceneMgr->createSceneNode();
	scnTorreBase -> attachObject(entTorreBase);
	scnTorreBase -> scale(1.5,3.0,1.5);
	scnTorreBase -> translate(0.0,300.0,0.0);
	scnTorre -> addChild(scnTorreBase);
	for(int i = 0; i < 7 ; ++i){
		Entity* entTorreAnillo = sceneMgr -> createEntity("poly04.mesh");
		entTorreAnillo -> setMaterialName("Examples/BumpyMetal");
		SceneNode* scnTorreAnillo = sceneMgr -> createSceneNode();
		scnTorreAnillo -> attachObject(entTorreAnillo);
		scnTorreAnillo -> translate(0.0,i*100.0,0.0);
		scnTorre -> addChild(scnTorreAnillo);
	}
	Entity* entTorreTope = sceneMgr -> createEntity("poly16.mesh");
	entTorreTope -> setMaterialName("Examples/BumpyMetal");
	SceneNode* scnTorreTope = sceneMgr -> createSceneNode();
	scnTorreTope -> attachObject(entTorreTope);
	
	scnTorreTope -> translate(0.0,600.0,0.0);
	scnTorreTope -> scale(1.5,1.5,1.5);

	scnTorre -> addChild(scnTorreTope);

	

	return scnTorre;

}

SceneNode* construirRejas(SceneManager* sceneMgr){

	SceneNode* nodoTorre1 = construirTorre(sceneMgr);
	nodoTorre1 -> translate(600.0,0.0,0.0);
	SceneNode* nodoTorre2 = construirTorre(sceneMgr);
	nodoTorre2 -> translate(-600.0,0.0,0.0);
	SceneNode* nodoReja = sceneMgr -> createSceneNode();

	SceneNode* barras = sceneMgr -> createSceneNode();
	Quaternion rotacionBarra(Degree(90.0),Vector3::UNIT_Y);
	for(int i =0 ; i < 6; ++i){
		SceneNode* nodoBarra = sceneMgr -> createSceneNode();
		Entity* barra = sceneMgr -> createEntity("Poly02.mesh");
		barra -> setMaterialName("Examples/BumpyMetal");
		nodoBarra -> attachObject(barra);
		nodoBarra -> translate(0.0,50.0+i*100,0.0);
		nodoBarra -> rotate(rotacionBarra);
		nodoBarra -> scale(0.2,0.1,12.0);
		
		barras -> addChild(nodoBarra);

	}

	nodoReja -> addChild(nodoTorre1);
	nodoReja -> addChild(nodoTorre2);
	nodoReja -> addChild(barras);
	nodoReja -> translate(0.0,0.0,-1200.0);

	
	

	return nodoReja;
	


}



SceneNode* construirPilares(SceneManager* sceneMgr){
	SceneNode* pilares = sceneMgr -> createSceneNode();

	return pilares;

}

Sierra* construirSierra(SceneManager* sceneMgr){
	// Erik para cuando leas
	// en el obstaculo puedes hacer algo asi:
	// new Obstacle(nodoSierra, Vector3::UNIT_X, 200.0f, Vector3::UNIT_X, 100.0f);
	// Esto es que rote en el eje X, y se translate en el eje X
	Entity* entSierra = sceneMgr ->createEntity("saw.mesh");
	SceneNode* nodoSierra = sceneMgr -> createSceneNode();
	nodoSierra->attachObject(entSierra);
	Sierra* sierra= new Sierra(50.0,nodoSierra,Vector3::UNIT_X,200.0);
	nodoSierra -> scale(2.0,2.0,7.0);
	return sierra;
}


class Starfox : public ExampleApplication {

public:
	Player *cPlayer;
	MainLoopFL *mainLoop;

	Starfox() {
	}

	~Starfox() {
		if (mainLoop)
			delete mainLoop;
		if (cPlayer)
			delete cPlayer;
	}

	void createFrameListener() {
		mainLoop = new MainLoopFL(cPlayer);
		mRoot->addFrameListener(mainLoop);
	}

	void createCamera() {
		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(0, 50, 300);
		mCamera->lookAt(0, 0, 0);
		mCamera->setNearClipDistance(5);
	}


	void chooseSceneManager() {
		mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");

		if (mOverlaySystem)
			mSceneMgr->addRenderQueueListener(mOverlaySystem);
	}

	void createScene() {
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
		SceneNode* reja = construirRejas(mSceneMgr);
		mSceneMgr -> getRootSceneNode() -> addChild(reja);
		obstacles[0]=new Obstacle(reja,Vector3::UNIT_Y,10.0);
		//
		//Construccion de las sierras
		Sierra* sierraPrueba = construirSierra(mSceneMgr);
		mSceneMgr -> getRootSceneNode() -> addChild(sierraPrueba -> _node);
		obstacles.push_back(sierraPrueba);
		//
		//Construccion de tercer obstaculo
		SceneNode* pilares = construirPilares(mSceneMgr);
		Plane plane(Vector3::UNIT_Y,0.0);
		MeshManager::getSingleton().createPlane("plane",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane,20000,90100,20,20,true,1,5,5,Vector3::UNIT_Z);

		Ogre::Entity* entPlano = mSceneMgr->createEntity("PlanoEntity", "plane");
		SceneNode* nodoPlano = mSceneMgr -> createSceneNode();
		entPlano -> setMaterialName("Examples/GrassFloor");
		nodoPlano -> attachObject(entPlano);
		nodoPlano -> translate(0.0,-5000.0,-44900.0);
		mSceneMgr->getRootSceneNode()->addChild(nodoPlano);
		


		

	}
};


int main (void) {
	Starfox app;
	app.go();
	return 0;
}