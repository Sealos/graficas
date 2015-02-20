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

int teclas = 0;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)


using namespace std;

#define DEF_floorGridScale  1.0
#define DEF_floorGridXSteps 10.0
#define DEF_floorGridZSteps 10.0

GLfloat lightPos[4] = {0.f, 200.f, 0.f, 1.f};
GLfloat lightLookAt[3] = {0.f, -1.f, 0.f};
GLfloat cutOff = 50.f;
GLfloat exponent = 25.f;
float shininess = 75.0f;

GLfloat rabbitColor[4] = {1.0f, 0.8f, 0.6f, 1.0f};

GLfloat ambiental[4] = {0.3f, 0.3f, 0.3f, 1.0f};
GLfloat specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat rabbitAmbiental[4] = {0.3f, 0.3f, 0.3f, 1.0f};
GLfloat rabbitSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat rabbitDiffuse[4] = {1.0f, 0.8f, 0.6f, 1.0f};

GLfloat ambientScalar = 1.f;

bool reflection = true;
bool ilumination = true;

GLuint vA;

GLfloat lightColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightAmbient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat diffuseScalar = 1.f;

unsigned char* images[3] = {nullptr, nullptr, nullptr};
unsigned char* cube_map[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

unsigned char* skybox[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
static GLuint skybox_tex[6];
int skyheight[6], skywidth[6];
int cmHeight[6], cmWidth[6];
int iheight[3], iwidth[3];
static GLuint texName[3];

static GLuint texPosX;
static GLuint texNegX;
static GLuint texPosY;
static GLuint texNegY;
static GLuint texPosZ;
static GLuint texNegZ;

static GLuint texObjPosX;
static GLuint texObjNegX;
static GLuint texObjPosY;
static GLuint texObjNegY;
static GLuint texObjPosZ;
static GLuint texObjNegZ;

#include "glm.h"


void changeViewport(int w, int h) {
	if (h == 0)
	{ h = 1; }

	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (GLfloat) w / (GLfloat) h, 1.0, 3000.0);
	glMatrixMode(GL_MODELVIEW);

}

void init() {

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);


	glGenTextures(1, &texName[0]);
	glBindTexture(GL_TEXTURE_2D, texName[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	images[0] = glmReadPPM("texAO_plano.ppm", &iwidth[0], &iheight[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth[0], iheight[0], 0, GL_RGB, GL_UNSIGNED_BYTE, images[0]);

	glGenTextures(1, &texName[1]);
	glBindTexture(GL_TEXTURE_2D, texName[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	images[1] = glmReadPPM("texAO_columna.ppm", &iwidth[1], &iheight[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth[1], iheight[1], 0, GL_RGB, GL_UNSIGNED_BYTE, images[1]);

	glGenTextures(1, &texName[2]);
	glBindTexture(GL_TEXTURE_2D, texName[2]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	images[2] = glmReadPPM("texAO_bunny.ppm", &iwidth[2], &iheight[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth[2], iheight[2], 0, GL_RGB, GL_UNSIGNED_BYTE, images[2]);

	glDisable(GL_TEXTURE_2D);


	//Cube Map

	cube_map[0] = glmReadPPM("posx.ppm", &cmHeight[0], &cmWidth[0]);
	cube_map[1] = glmReadPPM("negx.ppm", &cmHeight[1], &cmWidth[1]);
	cube_map[2] = glmReadPPM("posy.ppm", &cmHeight[2], &cmWidth[2]);
	cube_map[3] = glmReadPPM("negy.ppm", &cmHeight[3], &cmWidth[3]);
	cube_map[4] = glmReadPPM("posz.ppm", &cmHeight[4], &cmWidth[4]);
	cube_map[5] = glmReadPPM("negz.ppm", &cmHeight[5], &cmWidth[5]);

	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

	glGenTextures(1, &texPosX);
	glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, texPosX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, cmWidth[0], cmHeight[0], 1, GL_RGB, GL_UNSIGNED_BYTE, cube_map[0]);

	glGenTextures(1, &texNegX);
	glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, texNegX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, cmWidth[1], cmHeight[1], 1, GL_RGB, GL_UNSIGNED_BYTE, cube_map[1]);

	glGenTextures(1, &texPosY);
	glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, texPosY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, cmWidth[2], cmHeight[2], 1, GL_RGB, GL_UNSIGNED_BYTE, cube_map[2]);

	glGenTextures(1, &texNegY);
	glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, texNegY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, cmWidth[3], cmHeight[3], 1, GL_RGB, GL_UNSIGNED_BYTE, cube_map[3]);

	glGenTextures(1, &texPosZ);
	glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, texPosZ);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, cmWidth[4], cmHeight[4], 1, GL_RGB, GL_UNSIGNED_BYTE, cube_map[4]);

	glGenTextures(1, &texNegZ);
	glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, texNegZ);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, cmWidth[5], cmHeight[5], 1, GL_RGB, GL_UNSIGNED_BYTE, cube_map[5]);

	glGenTextures(1, &skybox_tex[0]);
	glBindTexture(GL_TEXTURE_2D, skybox_tex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cmWidth[0], cmHeight[0], 0, GL_RGB, GL_UNSIGNED_BYTE, cube_map[0]);

	glGenTextures(1, &skybox_tex[1]);
	glBindTexture(GL_TEXTURE_2D, skybox_tex[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cmWidth[1], cmHeight[1], 0, GL_RGB, GL_UNSIGNED_BYTE, cube_map[1]);

	glGenTextures(1, &skybox_tex[2]);
	glBindTexture(GL_TEXTURE_2D, skybox_tex[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cmWidth[0], cmHeight[2], 0, GL_RGB, GL_UNSIGNED_BYTE, cube_map[2]);

	glGenTextures(1, &skybox_tex[3]);
	glBindTexture(GL_TEXTURE_2D, skybox_tex[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cmWidth[0], cmHeight[3], 0, GL_RGB, GL_UNSIGNED_BYTE, cube_map[3]);

	glGenTextures(1, &skybox_tex[4]);
	glBindTexture(GL_TEXTURE_2D, skybox_tex[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cmWidth[0], cmHeight[4], 0, GL_RGB, GL_UNSIGNED_BYTE, cube_map[4]);

	glGenTextures(1, &skybox_tex[5]);
	glBindTexture(GL_TEXTURE_2D, skybox_tex[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cmWidth[0], cmHeight[5], 0, GL_RGB, GL_UNSIGNED_BYTE, cube_map[5]);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void cargar_materiales(int idx) {

	// Material Piso
	if (idx == 0) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambiental);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}

	// Material Columna
	if (idx == 1) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambiental);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}

	// Material Conejo
	if (idx == 2) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, rabbitAmbiental);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, rabbitDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, rabbitSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}
}

void recursive_render(const aiScene* sc, const aiNode* nd) {
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

			switch (face->mNumIndices) {
				case 1:
					face_mode = GL_POINTS;
					break;
				case 2:
					face_mode = GL_LINES;
					break;
				case 3:
					face_mode = GL_TRIANGLES;
					break;
				default:
					face_mode = GL_POLYGON;
					break;
			}

			glBegin(face_mode);

			for (i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];

				if (mesh->mColors[0] != NULL)
				{ glColor4fv((GLfloat*)&mesh->mColors[0][index]); }

				if (mesh->mNormals != NULL)
				{ glNormal3fv(&mesh->mNormals[index].x); }

				if (mesh->HasTextureCoords(0))
				{ glTexCoord2f(mesh->mTextureCoords[0][index].x, 1 - mesh->mTextureCoords[0][index].y); }

				glVertex3fv(&mesh->mVertices[index].x);
			}

			glEnd();

		}

	}
	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n) {

		glBindTexture(GL_TEXTURE_2D, texName[n]);
		cargar_materiales(n);
		recursive_render(sc, nd->mChildren[n]);
	}

	glPopMatrix();
}


void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			exit(0);
			break;
		// cutoff spotlight
		case 'q':
			cutOff += 1.f;
			break;
		case 'w':
			cutOff -= 1.f;
			if (cutOff < 0.0f)
			{ cutOff = 0.0f; }
			break;

		// exponent spotlight
		case 'a':
			exponent += 1.f;
			break;
		case 's':
			exponent -= 1.f;
			if (exponent < 0.0f)
			{ exponent = 0.0f; }
			break;

		// Ambiental of each model
		case 'z':
			ambientScalar += 0.05f;
			if (ambientScalar > 1.f)
			{ ambientScalar = 1.f; }

			ambiental[0] += 0.05f;
			ambiental[1] += 0.05f;
			ambiental[2] += 0.05f;

			rabbitAmbiental[0] += 0.05f;
			rabbitAmbiental[1] += 0.05f;
			rabbitAmbiental[2] += 0.05f;

			break;
		case 'x':
			ambientScalar -= 0.05f;
			if (ambientScalar < 0.f)
			{ ambientScalar = 0.f; }
			ambiental[0] -= 0.05f;
			ambiental[1] -= 0.05f;
			ambiental[2] -= 0.05f;

			rabbitAmbiental[0] -= 0.05f;
			rabbitAmbiental[1] -= 0.05f;
			rabbitAmbiental[2] -= 0.05f;
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
			{ rabbitColor[0] = 1.f; }

			rabbitAmbiental[0] = ambiental[0] * rabbitColor[0];
			rabbitDiffuse[0] = diffuse[0] * rabbitColor[0];
			rabbitSpecular[0] = specular[0] * rabbitColor[0];
			break;
		case 'g':
			rabbitColor[0] -= 0.05f;
			if (rabbitColor[0] < 0.f)
			{ rabbitColor[0] = 0.f; }
			rabbitAmbiental[0] = ambiental[0] * rabbitColor[0];
			rabbitDiffuse[0] = diffuse[0] * rabbitColor[0];
			rabbitSpecular[0] = specular[0] * rabbitColor[0];
			break;

		// Green bunny
		case 'y':
			rabbitColor[1] += 0.05f;
			if (rabbitColor[1] > 1.f)
			{ rabbitColor[1] = 1.f; }
			rabbitAmbiental[1] = ambiental[1] * rabbitColor[1];
			rabbitDiffuse[1] = diffuse[1] * rabbitColor[1];
			rabbitSpecular[1] = specular[1] * rabbitColor[1];
			break;
		case 'h':
			rabbitColor[1] -= 0.05f;
			if (rabbitColor[1] < 0.f)
			{ rabbitColor[1] = 0.f; }
			rabbitAmbiental[1] = ambiental[1] * rabbitColor[1];
			rabbitDiffuse[1] = diffuse[1] * rabbitColor[1];
			rabbitSpecular[1] = specular[1] * rabbitColor[1];
			break;

		// Blue bunny
		case 'u':
			rabbitColor[2] += 0.05f;
			if (rabbitColor[2] > 1.f)
			{ rabbitColor[2] = 1.f; }
			rabbitAmbiental[2] = ambiental[2] * rabbitColor[2];
			rabbitDiffuse[2] = diffuse[2] * rabbitColor[2];
			rabbitSpecular[2] = specular[2] * rabbitColor[2];
			break;
		case 'j':
			rabbitColor[2] -= 0.05f;
			if (rabbitColor[2] < 0.f)
			{ rabbitColor[2] = 0.f; }
			rabbitAmbiental[2] = ambiental[2] * rabbitColor[2];
			rabbitDiffuse[2] = diffuse[2] * rabbitColor[2];
			rabbitSpecular[2] = specular[2] * rabbitColor[2];
			break;

		// Reflection
		case 'c':
			reflection = !reflection;
			if (reflection) {
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_TEXTURE_CUBE_MAP);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);
			}
			else {
				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_CUBE_MAP);
				glEnable(GL_TEXTURE_2D);
			}


			break;
		// Only reflection no ilumination
		case 'v':
			ilumination = !ilumination;
			break;

		// Light
		case 'b':
			diffuseScalar += 0.05f;
			if (diffuseScalar > 1.f)
			{ diffuseScalar = 1.f; }

			/*diffuse[0] = diffuseScalar;
			diffuse[1] = diffuseScalar;
			diffuse[2] = diffuseScalar;

			rabbitDiffuse[0] = diffuse[0] * diffuseScalar * rabbitColor[0];
			rabbitDiffuse[1] = diffuse[1] * diffuseScalar * rabbitColor[1];
			rabbitDiffuse[2] = diffuse[2] * diffuseScalar * rabbitColor[2];*/

			lightColor[0] += diffuseScalar;
			lightColor[1] += diffuseScalar;
			lightColor[2] += diffuseScalar;

			lightSpecular[0] += diffuseScalar;
			lightSpecular[1] += diffuseScalar;
			lightSpecular[2] += diffuseScalar;

			lightAmbient[0] += diffuseScalar;
			lightAmbient[1] += diffuseScalar;
			lightAmbient[2] += diffuseScalar;



			break;
		case 'n':
			diffuseScalar -= 0.05f;
			if (diffuseScalar < 0.0f)
			{ diffuseScalar = 0.0f; }
			/*diffuse[0] = diffuseScalar;
			diffuse[1] = diffuseScalar;
			diffuse[2] = diffuseScalar;

			rabbitDiffuse[0] = diffuse[0] * diffuseScalar * rabbitColor[0];
			rabbitDiffuse[1] = diffuse[1] * diffuseScalar * rabbitColor[1];
			rabbitDiffuse[2] = diffuse[2] * diffuseScalar * rabbitColor[2];*/

			lightColor[0] -= diffuseScalar;
			lightColor[1] -= diffuseScalar;
			lightColor[2] -= diffuseScalar;

			lightSpecular[0] -= diffuseScalar;
			lightSpecular[1] -= diffuseScalar;
			lightSpecular[2] -= diffuseScalar;

			lightAmbient[0] -= diffuseScalar;
			lightAmbient[1] -= diffuseScalar;
			lightAmbient[2] -= diffuseScalar;

			break;

		// Light color
		case '1':
			lightColor[0] = 1.f;
			lightColor[1] = 1.f;
			lightColor[2] = 1.f;
			break;
		case '2':
			lightColor[0] = 1.f;
			lightColor[1] = 0.f;
			lightColor[2] = 0.f;
			break;
		case '3':
			lightColor[0] = 0.f;
			lightColor[1] = 1.f;
			lightColor[2] = 0.f;
			break;
		case '4':
			lightColor[0] = 0.f;
			lightColor[1] = 0.f;
			lightColor[2] = 1.f;
			break;
		case '5':
			lightColor[0] = 1.f;
			lightColor[1] = 0.f;
			lightColor[2] = 1.f;
			break;
	}
	scene_list = 0;
	glutPostRedisplay();
}

void render() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0, 80, 250, 0.0, 15.0, 0.0, 0.0, 1.0, 0.0);

	//Luz
	if (ilumination) {
		glEnable(GL_LIGHT0);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutOff);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightLookAt);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	}
	else {
		glDisable(GL_LIGHT0);
	}

	// Skybox
	glPushMatrix();

	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	//glColor3f(0.0f, 0.0f, 0.0f);

	// Front - Neg Z
	glBindTexture(GL_TEXTURE_2D, skybox_tex[5]);
	glBegin(GL_QUADS);
	{
		glVertex3f(+1.00f, -1.00f, -1.00f);
		glVertex3f(+1.00f, +1.00f, -1.00f);
		glVertex3f(-1.00f, +1.00f, -1.00f);
		glVertex3f(-1.00f, -1.00f, -1.00f);
	} 
	glEnd();

	// Left - Neg X
	glBindTexture(GL_TEXTURE_2D, skybox_tex[1]);
	glBegin(GL_QUADS);
	{
		glVertex3f(-1.00f, -1.00f, -1.00f);
		glVertex3f(-1.00f, +1.00f, -1.00f);
		glVertex3f(-1.00f, +1.00f, +1.00f);
		glVertex3f(-1.00f, -1.00f, +1.00f);
	}
	glEnd();

	// Right - Pos X
	glBindTexture(GL_TEXTURE_2D, skybox_tex[0]);
	glBegin(GL_QUADS);
	{
		glVertex3f(+1.00f, -1.00f, +1.00f);
		glVertex3f(+1.00f, -1.00f, -1.00f);
		glVertex3f(+1.00f, +1.00f, -1.00f);
		glVertex3f(+1.00f, +1.00f, +1.00f);
	}
	glEnd();

	// Back -- Cant see
	/*glBindTexture(GL_TEXTURE_2D, skybox_tex[1]);
	glBegin(GL_QUADS);
	{
		glVertex3f(-1.00f, -1.00f,  1.00f);
		glVertex3f(1.00f, -1.00f,  1.00f);
		glVertex3f(1.00f,  1.00f,  1.00f);
		glVertex3f(-1.00f,  1.00f,  1.00f);
	}
	glEnd();*/

	//Top -- cant see
	/*glBindTexture(GL_TEXTURE_2D, skybox_tex[4]);
	glBegin(GL_QUADS);
	{
		glVertex3f(-1.00f,  1.00f, -1.00f);
		glVertex3f(-1.00f,  1.00f,  1.00f);
		glVertex3f(1.00f,  1.00f,  1.00f);
		glVertex3f(1.00f,  1.00f, -1.00f);
	}
	glEnd();*/

	// Bot -- cant see
	/*glBindTexture(GL_TEXTURE_2D, skybox_tex[5]);
	glBegin(GL_QUADS);
	{
		glVertex3f(-1.00f, -1.00f, -1.00f);
		glVertex3f(-1.00f, -1.00f,  1.00f);
		glVertex3f(1.00f, -1.00f,  1.00f);
		glVertex3f(1.00f, -1.00f, -1.00f);
	}
	glEnd();*/

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	//Suaviza las lineas
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glPopAttrib();
	glPopMatrix();

	glPushMatrix();
	glEnable(GL_NORMALIZE);

	if (scene_list == 0) {
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

	glutTimerFunc(10, animacion, 1);
	glutPostRedisplay();

}

void get_bounding_box_for_node(const aiNode* nd,
                               aiVector3D* min,
                               aiVector3D* max,
                               aiMatrix4x4* trafo
                              ) {
	aiMatrix4x4 prev;
	unsigned int n = 0, t;


	prev = *trafo;
	aiMultiplyMatrix4(trafo, &nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene01->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp, trafo);

			min->x = aisgl_min(min->x, tmp.x);
			min->y = aisgl_min(min->y, tmp.y);
			min->z = aisgl_min(min->z, tmp.z);

			max->x = aisgl_max(max->x, tmp.x);
			max->y = aisgl_max(max->y, tmp.y);
			max->z = aisgl_max(max->z, tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n], min, max, trafo);
	}
	*trafo = prev;


}

void get_bounding_box(aiVector3D* min, aiVector3D* max) {
	aiMatrix4x4 trafo;


	aiIdentityMatrix4(&trafo);

	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene01->mRootNode, min, max, &trafo);


}

int loadasset(const char* path) {
	// we are taking one of the postprocessing presets to avoid
	// spelling out 20+ single postprocessing flags here.


	scene01 = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene01) {
		get_bounding_box(&scene_min, &scene_max);
		scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
		scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
		scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
		return 0;
	}

	return 1;
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960, 540);

	glutCreateWindow("Test Opengl");


	aiLogStream stream;
	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It remains active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
	aiAttachLogStream(&stream);

	// ... same procedure, but this stream now writes the
	// log messages to assimp_log.txt
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "assimp_log.txt");
	aiAttachLogStream(&stream);

	// the model name can be specified on the command line. If none
	// is specified, we try to locate one of the more expressive test
	// models from the repository (/models-nonbsd may be missing in
	// some distributions so we need a fallback from /models!).
	//if( 0 != loadasset( argc >= 2 ? argv[1] : "dragon_vrip_res2.ply")) {
	//  if( argc != 1 || (0 != loadasset( "dragon_vrip_res2.ply") && 0 != loadasset( "dragon_vrip_res2.ply"))) {
	//      return -1;
	//  }
	//}

	if (loadasset("escenario.obj") != 0) {
		return -1;
	}

	init();

	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
	return 0;

}
