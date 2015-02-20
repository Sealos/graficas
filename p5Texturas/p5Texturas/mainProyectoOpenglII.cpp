// Cubica

#include <stdlib.h>

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// the global Assimp scene object
const aiScene* scene01 = NULL;
const aiScene* scene02 = NULL;
const aiScene* scene03 = NULL;

GLuint scene_list = 0;
aiVector3D scene_min, scene_max, scene_center;



#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)


using namespace std;

#define DEF_floorGridScale	1.0
#define DEF_floorGridXSteps	10.0
#define DEF_floorGridZSteps	10.0

GLfloat lightPos[4] = {0.f, 200.f, 0.f, 1.f};
GLfloat lightLookAt[3] = {0.f, -1.f, 0.f};
GLfloat cutOff = 50.f;
GLfloat exponent = 25.f;

GLfloat rabbitColor[3] = {1.0f, 1.0f, 1.0f};

GLfloat ambientScale = 1.f;

bool reflection = true;
bool ilumination = true;

GLfloat lightColor[3] = {1.0f, 1.0f, 1.0f};
GLfloat lightIntensity = 1.f;

unsigned char* images[3] = {nullptr, nullptr, nullptr};
int iheight[3], iwidth[3];
static GLuint texName[3];

#include "glm.h"


void changeViewport(int w, int h) {
	if (h==0)
		h=1;

   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(30, (GLfloat) w/(GLfloat) h, 1.0, 3000.0);
   glMatrixMode (GL_MODELVIEW);

}

void init(){

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	glGenTextures(1, &texName[0]);
	glBindTexture(GL_TEXTURE_2D, texName[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	images[0] = glmReadPPM("texAO_plano.ppm", &iwidth[0], &iheight[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth[0], iheight[0], 0, GL_RGB, GL_UNSIGNED_BYTE, images[0]);

	glGenTextures(1, &texName[2]);
	glBindTexture(GL_TEXTURE_2D, texName[2]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	images[2] = glmReadPPM("texAO_bunny.ppm", &iwidth[2], &iheight[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth[2], iheight[2], 0, GL_RGB, GL_UNSIGNED_BYTE, images[2]);


	glGenTextures(1, &texName[1]);
	glBindTexture(GL_TEXTURE_2D, texName[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	images[1] = glmReadPPM("texAO_columna.ppm", &iwidth[1], &iheight[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth[1], iheight[1], 0, GL_RGB, GL_UNSIGNED_BYTE, images[1]);
	glDisable(GL_TEXTURE_2D);
}

void cargar_materiales(int idx) {
   
	// Material Piso
	if (idx == 0){
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, texName[0] );
		glDisable(GL_TEXTURE_2D);
	}

	// Material Columna
	if (idx == 1){
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, texName[1] );
		glDisable(GL_TEXTURE_2D);
	}

	// Material Conejo
	if (idx == 2){
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, texName[2] );
		glDisable(GL_TEXTURE_2D);
	}
}

void recursive_render (const aiScene *sc, const aiNode* nd)
{
	unsigned int i;
	unsigned int n = 0, t;
	aiMatrix4x4 m = nd->mTransformation;

	// update transform
	aiTransposeMatrix4(&m);
	glPushMatrix();
	glMultMatrixf((float*)&m);

	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n) {
		
		const aiMesh* mesh = scene01->mMeshes[nd->mMeshes[n]];

		for (t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch(face->mNumIndices) {
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for(i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				
				if(mesh->mColors[0] != NULL)
					glColor4fv((GLfloat*)&mesh->mColors[0][index]);
				
				if(mesh->mNormals != NULL) 
					glNormal3fv(&mesh->mNormals[index].x);
				
				if (mesh->HasTextureCoords(0)) 
					glTexCoord2f(mesh->mTextureCoords[0][index].x, 1-mesh->mTextureCoords[0][index].y);
				
				glVertex3fv(&mesh->mVertices[index].x);
			}

			glEnd();
		}

	}

	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n) {
		cargar_materiales(n);
		recursive_render(sc, nd->mChildren[n]);
	}

	glPopMatrix();
}


void Keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
	case 27:             
		exit (0);
		break;
	// cutoff spotlight
	case 'q':
		cutOff += 1.f;
		break;
	case 'w':
		cutOff -= 1.f;
		break;

	// exponent spotlight
	case 'a':
		exponent += 1.f;
		break;
	case 's':
		exponent -= 1.f;
		break;

	// Ambiental of each model
	case 'z':
		ambientScale += 0.05f;
		if (ambientScale > 1.f)
			ambientScale = 1.f;
		break;
	case 'x':
		ambientScale -= 0.05f;
		if (ambientScale < 0.f)
			ambientScale = 0.f;
		break;

	// X + -
	case 'e':
		lightPos[0] += 1.f;
		break;
	case 'd':
		lightPos[0] -= 1.f;
		break;

	// Z + -
	case 'r':
		lightPos[2] += 1.f;
		break;
	case 'f':
		lightPos[2] -= 1.f;
		break;

	// Red bunny
	case 't':
		rabbitColor[0] += 0.05f;
		if (rabbitColor[0] > 1.f)
			rabbitColor[0] = 1.f;
		break;
	case 'g':
		rabbitColor[0] -= 0.05f;
		if (rabbitColor[0] < 0.f)
			rabbitColor[0] = 0.f;
		break;

	// Green bunny
	case 'y':
		rabbitColor[1] += 0.05f;
		if (rabbitColor[1] > 1.f)
			rabbitColor[1] = 1.f;
		break;
		break;
	case 'h':
		rabbitColor[1] -= 0.05f;
		if (rabbitColor[1] < 0.f)
			rabbitColor[1] = 0.f;
		break;

	// Blue bunny
	case 'u':
		rabbitColor[2] += 0.05f;
		if (rabbitColor[2] > 1.f)
			rabbitColor[2] = 1.f;
		break;
		break;
	case 'j':
		rabbitColor[2] -= 0.05f;
		if (rabbitColor[2] < 0.f)
			rabbitColor[2] = 0.f;
		break;

	// Reflection
	case 'c':
		reflection = !reflection;
		break;
	// Only reflection no ilumination
	case 'v':
		reflection = !reflection;
		ilumination = false;
		break;

	// Light 
	case 'b':
		lightIntensity += 1.f;
		break;
	case 'n':
		lightIntensity -= 1.f;
		if (lightIntensity < 0.0f)
		lightIntensity = 0.0f;
		break;

	// Light color
	case '1':
		lightColor[0] = 1.f;
		lightColor[1] = 1.f;
		lightColor[2] = 1.f;
		break;
	case '2':
		break;
	case '3':
		break;
	case '4':
		break;
	case '5':
		break;
  }

  scene_list = 0;
  glutPostRedisplay();
}

void render(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();                       
	gluLookAt (0, 80, 250, 0.0, 15.0, 0.0, 0.0, 1.0, 0.0);
	
	//Luz
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutOff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightLookAt);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//Suaviza las lineas
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_LINE_SMOOTH );	

	glPushMatrix();
	glEnable(GL_NORMALIZE);
	if(scene_list == 0) {
	    scene_list = glGenLists(1);
	    glNewList(scene_list, GL_COMPILE);
            // now begin at the root node of the imported data and traverse
            // the scenegraph by multiplying subsequent local transforms
            // together on GL's matrix stack.		
	    recursive_render(scene01, scene01->mRootNode);
	    glEndList();
	}
	glCallList(scene_list);
	
	
	glPopMatrix();


	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glutSwapBuffers();
}

void animacion(int value) {
	
	glutTimerFunc(10,animacion,1);
    glutPostRedisplay();
	
}

void get_bounding_box_for_node (const aiNode* nd, 
	aiVector3D* min, 
	aiVector3D* max, 
	aiMatrix4x4* trafo
){
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	
		prev = *trafo;
		aiMultiplyMatrix4(trafo,&nd->mTransformation);

		for (; n < nd->mNumMeshes; ++n) {
			const aiMesh* mesh = scene01->mMeshes[nd->mMeshes[n]];
				for (t = 0; t < mesh->mNumVertices; ++t) {

				aiVector3D tmp = mesh->mVertices[t];
				aiTransformVecByMatrix4(&tmp,trafo);

				min->x = aisgl_min(min->x,tmp.x);
				min->y = aisgl_min(min->y,tmp.y);
				min->z = aisgl_min(min->z,tmp.z);

				max->x = aisgl_max(max->x,tmp.x);
				max->y = aisgl_max(max->y,tmp.y);
				max->z = aisgl_max(max->z,tmp.z);
			}
		}

		for (n = 0; n < nd->mNumChildren; ++n) {
			get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
		}
		*trafo = prev;
	

}

void get_bounding_box (aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 trafo;

	
		aiIdentityMatrix4(&trafo);
	
		min->x = min->y = min->z =  1e10f;
		max->x = max->y = max->z = -1e10f;
		get_bounding_box_for_node(scene01->mRootNode,min,max,&trafo);


}

int loadasset (const char* path)
{
	// we are taking one of the postprocessing presets to avoid
	// spelling out 20+ single postprocessing flags here.
	
		
		scene01 = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene01) {
			get_bounding_box(&scene_min,&scene_max);
			scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
			scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
			scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
			return 0;
		}

	return 1;
}

int main (int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960,540);

	glutCreateWindow("Test Opengl");


	aiLogStream stream;
	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It remains active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
	aiAttachLogStream(&stream);

	// ... same procedure, but this stream now writes the
	// log messages to assimp_log.txt
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
	aiAttachLogStream(&stream);

	// the model name can be specified on the command line. If none
	// is specified, we try to locate one of the more expressive test 
	// models from the repository (/models-nonbsd may be missing in 
	// some distributions so we need a fallback from /models!).
	//if( 0 != loadasset( argc >= 2 ? argv[1] : "dragon_vrip_res2.ply")) {
	//	if( argc != 1 || (0 != loadasset( "dragon_vrip_res2.ply") && 0 != loadasset( "dragon_vrip_res2.ply"))) { 
	//		return -1;
	//	}
	//}

	if (loadasset( "escenario.obj") != 0) {
		return -1;
	}




	init ();

	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutKeyboardFunc (Keyboard);
	


	glutMainLoop();
	return 0;

}
