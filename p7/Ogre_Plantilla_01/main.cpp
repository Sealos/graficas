#include "stdafx.h"
#include "player.h"
#include "coin.h"

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
	}
};


int main (void) {
	Starfox app;
	app.go();
	return 0;
}