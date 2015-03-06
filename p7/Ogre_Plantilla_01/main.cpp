#include "stdafx.h"
#include "player.h"
#include "coin.h"
#include "Ogre\ExampleApplication.h"

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
		return player->onUpdate(evt.timeSinceLastFrame);
	}
};

class Starfox : public ExampleApplication {

public:
	Player *cPlayer;
	MainLoopFL *mainLoop;

	Starfox() {
	}

	~Starfox() {
		if (mainLoop)
			delete mainLoop;
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
		SceneNode *player;
		SceneNode *padre;
		SceneNode *cameraNode;

		cameraNode = mSceneMgr->createSceneNode();
		cameraNode->attachObject(mCamera);
		mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));
		mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
		Entity *entEscenario01 = mSceneMgr->createEntity("EscenarioBase01", "proyectoOgreI.mesh");
		SceneNode *nodeEscenario01 = mSceneMgr->createSceneNode("NodeMesh01");
		mSceneMgr->getRootSceneNode()->addChild(nodeEscenario01);
		nodeEscenario01->attachObject(entEscenario01);
		//Luces
		Light *light1 = mSceneMgr->createLight("Light1");
		light1->setType(Light::LT_POINT);
		light1->setPosition(0, 0, 200);
		light1-> setDiffuseColour(0.8f, 0.8f, 0.8f);
		Entity *torus = mSceneMgr->createEntity("ObjetoPrueba", "usb_torus.mesh");
		Entity *cilindro =  mSceneMgr->createEntity("ObjetoPrueba1", "usb_cubomod01.mesh");
		torus->setMaterialName("lambert3");
		player = mSceneMgr->createSceneNode("player");
		//player->showBoundingBox(true);
		player->attachObject(torus);
		player->scale(3.0, 3.0, 3.0);
		padre = mSceneMgr->createSceneNode();
		padre->addChild(cameraNode);
		mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
		padre-> addChild(player);
		cPlayer = new Player(mSceneMgr, mWindow, player, padre);
		mSceneMgr->getRootSceneNode()->addChild(padre);
	}

};


int main (void) {
	Starfox app;
	app.go();
	return 0;
}