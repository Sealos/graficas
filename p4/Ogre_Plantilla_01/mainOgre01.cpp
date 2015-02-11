#include "Ogre\ExampleApplication.h"

class Example1 : public ExampleApplication
{
public:


	void createCamera() {

		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(60,10,60);
		mCamera->lookAt(0,0,0);
		mCamera->setNearClipDistance(5);

	}

	void createScene()
	{ 

		 mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));

		 
				
		Ogre::Entity* entMesh01 = mSceneMgr->createEntity("MyEntity1","ejes01.mesh");
		Ogre::SceneNode* node01 = mSceneMgr->createSceneNode("Node01");
		mSceneMgr->getRootSceneNode()->addChild(node01);
		node01->attachObject(entMesh01);


		Ogre::Plane plane(Vector3::UNIT_Y,0.0);
		Ogre::MeshManager::getSingleton().createPlane("plane",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane,1500,1500,20,20,true,1,5,5,Vector3::UNIT_Z);

		Ogre::Entity* entPLano = mSceneMgr->createEntity("PlanoEntity", "plane");
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entPLano);
		entPLano -> setMaterialName("Examples/BeachStones");

		Ogre::SceneNode* node02 = mSceneMgr->createSceneNode("NodeLuz");
		mSceneMgr->getRootSceneNode()->addChild(node02);

		Ogre::Light* Luz01 = mSceneMgr->createLight("Luz01");
		Luz01->setType(Ogre::Light::LT_POINT);
		Luz01-> setPosition(-20,20,0);
		Luz01 -> setDiffuseColour(0.0f,1.0f,0.0f);

		

		Ogre::Entity* spLight01 = mSceneMgr->createEntity("MyEntitySphereLuz", "sphere.mesh");
		Ogre::SceneNode* node04= node02-> createChildSceneNode("node04");

		node04->setScale(0.01,0.01,0.01);
		node04->setPosition(-20,20,0);


		node04->attachObject(spLight01);


		Ogre::Light* Luz02 = mSceneMgr->createLight("Luz02");
		Luz02->setType(Ogre::Light::LT_POINT);
		Luz02-> setPosition(20,20,0);
		Luz02 -> setDiffuseColour(1.0f,0.0f,0.0f);

			Ogre::Entity* spLight02 = mSceneMgr->createEntity("MyEntitySphereLuz2", "sphere.mesh");
		Ogre::SceneNode* node05= node02-> createChildSceneNode("node05");

		node05->setScale(0.01,0.01,0.01);
		node05->setPosition(20,20,0);

		node05->attachObject(spLight02);

		
		/*
		Ogre::Entity* entMesh02 = mSceneMgr->createEntity("MyEntity2","cuadriculaX01.mesh");
		Ogre::SceneNode* node02 = mSceneMgr->createSceneNode("Node02");
		mSceneMgr->getRootSceneNode()->addChild(node02);
		node02->attachObject(entMesh02);

		*/
		//Objetos
		
		//Ogre::SceneNode* nodeEjeSinbad = mSceneMgr->createSceneNode("NodeEjeSinbad");
		Ogre::SceneNode* nodeSinbad = mSceneMgr->createSceneNode("NodeSinbad");
		//Ogre::SceneNode* nodeNinja = mSceneMgr->createSceneNode("NodeNinja");


		//Ogre::Entity* entEjeSinbad = mSceneMgr -> createEntity("MyEntityEjeSinbad", "ejes01.mesh");
		Ogre::Entity* entSinbad = mSceneMgr->createEntity("MyEntitySinbad","sinbad.mesh");
		//Ogre::Entity* entNinja = mSceneMgr -> createEntity("MyEntityNinja", "ninja.mesh");


		mSceneMgr -> getRootSceneNode()-> addChild(nodeSinbad);
		
		//nodeEjeSinbad->attachObject(entEjeSinbad);
		//nodeEjeSinbad->addChild(nodeSinbad);
		//nodeEjeSinbad->addChild(nodeNinja);

		//Hijo Sinbad
		//nodeSinbad -> setScale(3.0,3.0,3.0);
		nodeSinbad -> setPosition(0.0,4.0,0.0);
		//nodeSinbad -> setScale(0.8,0.8,0.8);
		//nodeSinbad ->translate(0.0,0.0,8.0,Ogre::Node::TS_WORLD); // <- para que el cambio ocurra en el eje del mundo en vez del padre
		


		nodeSinbad->attachObject(entSinbad);
		//nodeNinja->attachObject(entNinja);
		
		//Eje
		//nodeEjeSinbad -> setScale(3.0,3.0,3.0);
		//nodeEjeSinbad -> setPosition(5.0,4.0,0.0);
		//nodeEjeSinbad -> translate(2.0,0.0,2.0);
		//nodeEjeSinbad-> yaw(Ogre::Degree(45.0f)); //rota en Y
		//nodeEjeSinbad->pitch(Ogre::Degree(45.0f)); //rota en X
		//nodeEjeSinbad->roll(Ogre::Degree(45.0f)); //rota en Z

				//Hijo Ninja
		//nodeNinja->setScale(0.04,0.04,0.04);
		//nodeNinja-> yaw(Ogre::Degree(180.0f)); //rota en Y
		//nodeNinja->setPosition(4.0,-2.0,5.0);
		
		//node03->attachObject(entMesh03);
		//node03 -> attachObject(entMesh04);
		//node03->setScale(0.6,0.6,0.6);
		//node03->setPosition(5.0,0.0,0.0);

		

	

	}

};

class Example2 : public ExampleApplication
{
private:
	Ogre::SceneNode* _SinbadNode;
public:

	void createCamera(){
	
		mCamera = mSceneMgr -> createCamera("MyCamera1");
		mCamera -> setPosition(0,100,200);
		mCamera -> lookAt(0,0,0);
		mCamera -> setNearClipDistance(5);
	}

	void createScene(){
		//Create Plane
		/*
		Plane plane(Vector3::UNIT_Y, -10);
		Ogre::MeshManager::getSingleton().createPlane(
			"plane", 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			plane, 
			1500,1500,200,200,
			true,1,5,5,Vector3::UNIT_Z);
		//Create Entity for plane
		Ogre::Entity* ent = mSceneMgr->createEntity("LightPlaneEntity","plane");
		//Attaching the plane to the root scene
		mSceneMgr -> getRootSceneNode()-> createChildSceneNode() -> attachObject(ent);
		ent->setMaterialName("Examples/Beachstones");
		*/
		
		Ogre::Plane plane(Vector3::UNIT_Y,-10.0);
		Ogre::MeshManager::getSingleton().createPlane("plane",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane,1500,1500,20,20,true,1,5,5,Vector3::UNIT_Z);

		Ogre::Entity* entPLano = mSceneMgr->createEntity("PlanoEntity", "plane");
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entPLano);
		entPLano -> setMaterialName("Examples/BeachStones");

		Ogre::Light* light = mSceneMgr -> createLight("Light1");
		light -> setType(Ogre::Light::LT_DIRECTIONAL);
		light -> setDirection(Ogre::Vector3(1,-1,0));
		
		Ogre:: SceneNode* node = mSceneMgr -> createSceneNode("Node1");
		mSceneMgr -> getRootSceneNode() -> addChild(node);

		Ogre::Entity* Sinbad = mSceneMgr -> createEntity("Sinbad", "Sinbad.mesh");
		_SinbadNode = node -> createChildSceneNode("SinbadNode");
		_SinbadNode -> setScale(3.0f,3.0f,3.0f);
		_SinbadNode -> setPosition(Ogre::Vector3(0.0f,4.0f,0.0f));
		_SinbadNode -> attachObject(Sinbad);

		mSceneMgr -> setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);



	}

};

int main (void)
{

	Example2 app;
	app.go();
	return 0;
}