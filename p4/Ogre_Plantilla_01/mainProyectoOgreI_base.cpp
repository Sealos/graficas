#include "Ogre\ExampleApplication.h"
Ogre::SceneNode* torretas [8];


class TorretasFrameListener : public FrameListener{
public:
	Ogre::Timer laserTimes[8];
	int tiempos[8];

	TorretasFrameListener(){
		//Hacer RANDOM
		for(int i = 0; i < 8; ++i){
			laserTimes[i].reset();
			tiempos[i] = 2000;
		}
	}

	bool frameStarted(const Ogre::FrameEvent &evt){
		for(int i = 0; i < 8; ++i){
			if(laserTimes[i].getMilliseconds() > tiempos[i]){
				laserTimes[i].reset();
				std::cout << "BAM!\n";
				//DISPARAR LASER
			}
		}
		return true;
	}
};

inline SceneNode* crearTorreta(SceneManager* mSceneMgr,Vector3 vec){
	Ogre::Entity* ent = mSceneMgr -> createEntity("usb_cilindro.mesh");
	Ogre::SceneNode* nodo = mSceneMgr -> createSceneNode();
	nodo -> attachObject(ent);
	nodo -> translate(vec);
	return nodo;
}

class Example25FrameListener : public FrameListener{
private:
	Ogre::SceneNode* _playerNode;
	Ogre::SceneNode* _padreNode;
	Ogre::Camera* _cam;
	OIS::InputManager* _man;
	OIS::Keyboard* _key;
	OIS::Mouse* _mouse;


public:

	Example25FrameListener(Ogre::SceneNode* playerNode, Ogre::SceneNode* padre, RenderWindow* win, Ogre::Camera* cam){
		_playerNode = playerNode;
		_padreNode = padre;
		//Helper variables for extracting the window handle
		size_t windowHnd =0;
		std::stringstream windowHndStr;
		win -> getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		//Create the parameter list for OIS and add the handle 'windowHndStr' to it
		OIS::ParamList pl;
		pl.insert(std::make_pair(std::string("WINDOW"),windowHndStr.str()));
		//Create the input system
		_man = OIS::InputManager::createInputSystem(pl);
		_key = static_cast<OIS::Keyboard*>(_man->createInputObject(OIS::OISKeyboard,false));
		//Camera assignment
		_mouse = static_cast<OIS::Mouse*>(_man->createInputObject(OIS::OISMouse,false));
		_cam = cam;
	}

	~Example25FrameListener(){
			_man->destroyInputObject(_key);
			_man -> destroyInputObject(_mouse);
			OIS::InputManager::destroyInputSystem(_man);
	}

	bool frameStarted(const Ogre::FrameEvent &evt){
		Ogre::Vector3 translateCam(0.0,0.0,0.0);
		Ogre::Vector3 translatePlayer(0.0,0.0,0.0);
		Ogre::Quaternion rotatePlayer(Degree(0),Vector3::UNIT_Y);
		Ogre::Quaternion strafePlayer(Degree(0),Vector3::UNIT_X);
		float speedFactor = 2000.0f;
		float playerSpeed = 500.f;
		float rotationSpeed = 50.f;
		float rotationAmount = 1.f;
		float strafeRotSpeed = 200.f;

		bool reiniciarPosicion = false;
		_key->capture();
		_mouse -> capture();
		if(_key->isKeyDown(OIS::KC_ESCAPE)){
			return false;
		}
		//Camera Controls
		/*if(_key->isKeyDown(OIS::KC_T)){
			translateCam+=Ogre::Vector3(0.0,0.0,-1.0);
		}
		if(_key->isKeyDown(OIS::KC_G)){
			translateCam+=Ogre::Vector3(0.0,0.0,1.0);
		}
		if(_key->isKeyDown(OIS::KC_F)){
			translateCam+=Ogre::Vector3(-1,0.0,0.0);
		}
		if(_key->isKeyDown(OIS::KC_H)){
			translateCam+=Ogre::Vector3(1.0,0.0,0.0);
		}
		float rotX = _mouse->getMouseState().X.rel * evt.timeSinceLastFrame * -1;
		float rotY = _mouse->getMouseState().Y.rel * evt.timeSinceLastFrame* -1;
		_cam -> yaw(Ogre::Radian(rotX));
		_cam -> pitch(Ogre::Radian(rotY));
		_cam ->moveRelative(translateCam*evt.timeSinceLastFrame*speedFactor);
*/
		if(_key->isKeyDown(OIS::KC_1)){
			std::cout << _padreNode->getPosition() << "\n";
		}

		Ogre::Quaternion q;
		//Player Controls

		if(_key -> isKeyDown(OIS::KC_UP)){
			translatePlayer+=Ogre::Vector3(0.0,1.0,0.0);

		}
		if(_key -> isKeyDown(OIS::KC_DOWN)){
			translatePlayer+=Ogre::Vector3(0.0,-1.0,0.0);
		}

		if(_key->isKeyDown(OIS::KC_LEFT)){
			rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree((rotationAmount*rotationSpeed*evt.timeSinceLastFrame)),Vector3::UNIT_Y);
		}

		if(_key->isKeyDown(OIS::KC_RIGHT)){
			rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(-(rotationAmount*rotationSpeed*evt.timeSinceLastFrame)),Vector3::UNIT_Y);
		}

		if(_key->isKeyDown(OIS::KC_W)){
			q =_playerNode -> _getDerivedOrientation();
			Ogre::Radian angulo = q.getYaw();
			translatePlayer+=Ogre::Vector3(cos(angulo.valueRadians()),0.0,-sin(angulo.valueRadians()));
			
		}
		if(_key->isKeyDown(OIS::KC_A)){
			translatePlayer+=Ogre::Vector3(-1.0,0.0,0.0);
			if(_playerNode->getOrientation().getRoll().valueDegrees() < 45.0){
				strafePlayer = strafePlayer * Ogre::Quaternion(Degree(strafeRotSpeed*evt.timeSinceLastFrame),Vector3::UNIT_Z);
			}
		}else{
			if(_playerNode->getOrientation().getRoll().valueDegrees() > 0.1f){
				strafePlayer = strafePlayer * Ogre::Quaternion(Degree(-(strafeRotSpeed*evt.timeSinceLastFrame)),Vector3::UNIT_Z);

			}
		}
		if(_key->isKeyDown(OIS::KC_D)){
			translatePlayer+=Ogre::Vector3(1,0.0,0.0);
			if(_playerNode->getOrientation().getRoll().valueDegrees() > -45.f){
				strafePlayer = strafePlayer * Ogre::Quaternion(Degree(-(strafeRotSpeed*evt.timeSinceLastFrame)),Vector3::UNIT_Z);
			}
		}else{
			if(_playerNode->getOrientation().getRoll().valueDegrees() < -0.1f){
				strafePlayer = strafePlayer * Ogre::Quaternion(Degree(strafeRotSpeed*evt.timeSinceLastFrame),Vector3::UNIT_Z);

			}
		}

		
		/*if(!(_key->isKeyDown(OIS::KC_A) || _key->isKeyDown(OIS::KC_D))){
			if(_playerNode->getOrientation().getRoll().valueDegrees() > 0.f){
				rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(-(rotationAmount*rotationSpeed*evt.timeSinceLastFrame)),Vector3::UNIT_Z);

			}else if(_playerNode->getOrientation().getRoll().valueDegrees() < 0.f){
				rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(rotationAmount*rotationSpeed*evt.timeSinceLastFrame),Vector3::UNIT_Z);

			}

		}*/
		_padreNode -> translate(translatePlayer*evt.timeSinceLastFrame*playerSpeed);
		//_cam->move(translatePlayer*evt.timeSinceLastFrame*playerSpeed);
		//_cam->rotate(rotatePlayer);
		/*if(_playerNode->getOrientation().getRoll().valueDegrees() > -1.f && _playerNode->getOrientation().getRoll().valueDegrees() < 1.f){
			Ogre::Vector3 orientacionActual = _playerNode -> getOrientation() * Vector3::UNIT_Z;
			Ogre::Quaternion reset = orientacionActual.getRotationTo(Vector3::UNIT_Z);
			_playerNode -> rotate(reset);
		}else{*/
			_padreNode -> rotate(rotatePlayer);
			_playerNode -> rotate(strafePlayer);
		
		//}
		return true;
	}
};

class Example1 : public ExampleApplication
{

public:
	Ogre::FrameListener* FrameListener;
	Ogre::FrameListener* TorretaListener;
	Ogre::SceneNode* player;
	Ogre::SceneNode* padre;
	Ogre::SceneNode* cameraNode;

	Example1(){
		FrameListener = nullptr;
		TorretaListener = nullptr;
	}

	~Example1(){
		if(FrameListener){
			delete FrameListener;
		}
		if(TorretaListener){
			delete TorretaListener;
		}
	}

	void createFrameListener(){
		FrameListener = new Example25FrameListener(player,padre,mWindow,mCamera);
		TorretaListener = new TorretasFrameListener();
		mRoot -> addFrameListener(FrameListener);
		mRoot -> addFrameListener(TorretaListener);

	}

	void createCamera() {

		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(0,200,200);
		mCamera->lookAt(0,0,0);
		mCamera->setNearClipDistance(5);

	}

	void createScene()
	{

		

		cameraNode = mSceneMgr->createSceneNode("CameraNodo");
		cameraNode -> attachObject(mCamera);
	


		mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		

		Ogre::Entity* ent01 = mSceneMgr->createEntity("MyEntity1","ejes01.mesh");
		Ogre::SceneNode* node01 = mSceneMgr->createSceneNode("Node01");
		mSceneMgr->getRootSceneNode()->addChild(node01);
		node01->attachObject(ent01);
		

		Ogre::Entity* entEscenario01 = mSceneMgr->createEntity("EscenarioBase01","proyectoOgreI.mesh");
		Ogre::SceneNode* nodeEscenario01 = mSceneMgr->createSceneNode("NodeMesh01");		
		mSceneMgr->getRootSceneNode()->addChild(nodeEscenario01);
		nodeEscenario01->attachObject(entEscenario01);

		Ogre::Entity* torus = mSceneMgr->createEntity("ObjetoPrueba","usb_torus.mesh");
		Ogre::Entity* cilindro =  mSceneMgr->createEntity("ObjetoPrueba1","usb_cubomod01.mesh");
		player = mSceneMgr -> createSceneNode("player");
		player -> showBoundingBox(true);


		


		player -> attachObject(torus);
		player -> attachObject(cilindro);
		player -> scale(3.0,3.0,3.0);
		player -> rotate(Ogre::Quaternion(Degree(90),Vector3::UNIT_Y));

		//Ogre::SceneNode* nodo = crearTorreta(mSceneMgr,Ogre::Vector3(0.0,0.0,0.0));
		//mSceneMgr -> getRootSceneNode() -> addChild(nodo);


		padre = mSceneMgr -> createSceneNode();
		padre -> addChild(cameraNode);
		padre-> addChild(player);
		mSceneMgr -> getRootSceneNode() -> addChild(padre);



	}

};


int main (void)
{

	Example1 app;
	app.go();
	return 0;
}