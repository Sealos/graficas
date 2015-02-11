#include "Ogre\ExampleApplication.h"

class Example25FrameListener : public FrameListener{
private:
	Ogre::SceneNode* _playerNode;
	Ogre::Camera* _cam;
	OIS::InputManager* _man;
	OIS::Keyboard* _key;
	OIS::Mouse* _mouse;


public:

	Example25FrameListener(Ogre::SceneNode* playerNode, RenderWindow* win, Ogre::Camera* cam){
		_playerNode = playerNode;
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
		Ogre::Quaternion rotatePlayer(Degree(0),Vector3::UNIT_Z);
		float speedFactor = 2000.0f;
		float playerSpeed = 200.f;
		float rotationSpeed = 50.f;
		float rotationAmount = 5.f;
		bool reiniciarPosicion = false;
		_key->capture();
		_mouse -> capture();
		if(_key->isKeyDown(OIS::KC_ESCAPE)){
			return false;
		}
		//Camera Controls
		if(_key->isKeyDown(OIS::KC_T)){
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

		//Player Controls
		if(_key->isKeyDown(OIS::KC_W)){
			translatePlayer+=Ogre::Vector3(0.0,0.0,-1.0);
			
		}
		if(_key->isKeyDown(OIS::KC_A)){
			translatePlayer+=Ogre::Vector3(-1.0,0.0,0.0);
			if(_playerNode->getOrientation().getRoll().valueDegrees() < 45.0){
				rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(rotationAmount*rotationSpeed*evt.timeSinceLastFrame),Vector3::UNIT_Z);
			}
		}else{
			if(_playerNode->getOrientation().getRoll().valueDegrees() > 0.1f){
				rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(-(rotationAmount*rotationSpeed*evt.timeSinceLastFrame)),Vector3::UNIT_Z);

			}

		}
		if(_key->isKeyDown(OIS::KC_D)){
			translatePlayer+=Ogre::Vector3(1,0.0,0.0);
			if(_playerNode->getOrientation().getRoll().valueDegrees() > -45.f){
				rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(-(rotationAmount*rotationSpeed*evt.timeSinceLastFrame)),Vector3::UNIT_Z);
			}
		}else{
			if(_playerNode->getOrientation().getRoll().valueDegrees() < -0.1f){
				rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(rotationAmount*rotationSpeed*evt.timeSinceLastFrame),Vector3::UNIT_Z);

			}

		}
		/*if(!(_key->isKeyDown(OIS::KC_A) || _key->isKeyDown(OIS::KC_D))){
			if(_playerNode->getOrientation().getRoll().valueDegrees() > 0.f){
				rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(-(rotationAmount*rotationSpeed*evt.timeSinceLastFrame)),Vector3::UNIT_Z);

			}else if(_playerNode->getOrientation().getRoll().valueDegrees() < 0.f){
				rotatePlayer = rotatePlayer * Ogre::Quaternion(Degree(rotationAmount*rotationSpeed*evt.timeSinceLastFrame),Vector3::UNIT_Z);

			}

		}*/
		_playerNode -> translate(translatePlayer*evt.timeSinceLastFrame*playerSpeed);
		/*if(_playerNode->getOrientation().getRoll().valueDegrees() > -1.f && _playerNode->getOrientation().getRoll().valueDegrees() < 1.f){
			Ogre::Vector3 orientacionActual = _playerNode -> getOrientation() * Vector3::UNIT_Z;
			Ogre::Quaternion reset = orientacionActual.getRotationTo(Vector3::UNIT_Z);
			_playerNode -> rotate(reset);
		}else{*/
		
			_playerNode -> rotate(rotatePlayer);
		
		//}
		return true;
	}
};

class Example1 : public ExampleApplication
{

public:
	Ogre::FrameListener* FrameListener;
	Ogre::SceneNode* player;

	Example1(){
		FrameListener = NULL;
	}

	~Example1(){
		if(FrameListener){
			delete FrameListener;
		}
	}

	void createFrameListener(){
		FrameListener = new Example25FrameListener(player,mWindow,mCamera);
		mRoot -> addFrameListener(FrameListener);

	}

	void createCamera() {

		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(100,50,100);
		mCamera->lookAt(0,0,0);
		mCamera->setNearClipDistance(5);

	}

	void createScene()
	{

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
		player = mSceneMgr -> createSceneNode("player");
		mSceneMgr -> getRootSceneNode() -> addChild(player);
		player -> attachObject(torus);


	}

};


int main (void)
{

	Example1 app;
	app.go();
	return 0;
}