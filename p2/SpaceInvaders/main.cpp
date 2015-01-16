#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#include <algorithm>
#include <math.h>

using namespace std;

static const int SCREEN_HEIGHT = 600;
static const int SCREEN_WIDTH = 600;
static const float DEGREE = static_cast<float>(3.14159/180);
static const GLfloat wall = -0.2f;
GLfloat rotation[5] = { 15.0f, -60.0f, -100.0f , 0.0f, 0.0f };
GLfloat carPos = 0.0f;

#define X_TO_GLFLOAT(X) (X/SCREEN_WIDTH*2)
#define Y_TO_GLFLOAT(Y) (Y/SCREEN_HEIGHT*2)

void changeViewport(int w, int h)
{
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void drawFloor()
{
	glPushMatrix();
	glBegin(GL_LINES);
	{
		glVertex3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
	}
	glEnd();
	glPopMatrix();
}

void drawCircle(float radius, int j)
{
	glRotatef(rotation[j], 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINE_LOOP);

	for (int i=0; i < 360; i = i + 60)
	{
		float degInRad = i*DEGREE;
		float x = cos(degInRad)*radius;
		float y = sin(degInRad)*radius;
		glVertex3f(x, y, 0.0f);
	}
	glEnd();
}

void drawBar(float width, float height)
{
	glBegin(GL_LINE_LOOP);
	{
		glVertex3f(-width, 0.0f, 0.0f);
		glVertex3f(width, 0.0f, 0.0f);
		glVertex3f(width, height, 0.0f);
		glVertex3f(-width, height, 0.0f);
	}
	glEnd();
}

void drawTriangle()
{
	glBegin(GL_LINE_LOOP);
	{
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.12f, 0.2f, 0.0f);
		glVertex3f(-0.12f, 0.2f, 0.0f);
	}
	glEnd();
}

void drawLamp()
{
	glPushMatrix();
	glTranslatef(-0.6f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	{
		glVertex3f(-0.2f, 0.0f, 0.0f);
		glVertex3f(0.2f, 0.0f, 0.0f);
		glVertex3f(0.2f, 0.1f, 0.0f);
		glVertex3f(-0.2f, 0.1f, 0.0f);
	}
	glEnd();

	glTranslatef(0.0f, 0.08f, 0.0f);
	drawCircle(0.04f, 0);

	glPushMatrix();
	{
		glTranslatef(0.03f, 0.0f, 0.0f);
		drawBar(0.01f, 0.5f);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-0.03f, 0.0f, 0.0f);
		drawBar(0.01f, 0.5f);
	}
	glPopMatrix();

	glTranslatef(0.0f, 0.5f, 0.0f);
	drawCircle(0.04f, 1);
	drawBar(0.02f, 0.5f);
	glTranslatef(0.0f, 0.5f, 0.0f);
	drawCircle(0.04f, 2);
	glTranslatef(0.0f, 0.04f, 0.0f);
	drawTriangle();
	glPopMatrix();
}

void drawCar()
{
	glPushMatrix();
	{
		glTranslatef(carPos, 0.0f, 0.0f);
		glBegin(GL_LINE_LOOP);
		{
			glVertex3f(-0.2f, 0.05f, 0.0f);
			glVertex3f(0.2f, 0.05f, 0.0f);
			glVertex3f(0.2f, 0.10f, 0.0f);
			glVertex3f(-0.12f, 0.175f, 0.0f);
			glVertex3f(-0.2f, 0.14f, 0.0f);
		}
		glEnd();

		glPushMatrix();
		{
			glTranslatef(0.15f, 0.05f, 0.0f);
			drawCircle(0.05f, 3);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(-0.15f, 0.05f, 0.0f);
			drawCircle(0.05f, 3);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(-0.1f, 0.16f, 0.0f);
			drawCircle(0.03f, 5);
		}
		glPopMatrix();

	}
	glPopMatrix();

}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslatef(0.0f, -0.95f, 0.0f);
	drawFloor();
	drawLamp();
	drawCar();

	glutSwapBuffers();
	glutPostRedisplay();
}

void getKeyPress(unsigned char key, int x, int y)
{
	GLfloat newCarPos;
	GLfloat diff;
	switch (key)
	{
		case 'z':
			rotation[0] =  static_cast<GLfloat>(fmod(rotation[0] - 3.0f, 360));
			break;
		case 'x':
			rotation[0] =  static_cast<GLfloat>(fmod(rotation[0] + 3.0f, 360));
			break;
		case 'a':
			rotation[1] =  static_cast<GLfloat>(fmod(rotation[1] - 3.0f, 360));
			break;
		case 's':
			rotation[1] =  static_cast<GLfloat>(fmod(rotation[1] + 3.0f, 360));
			break;
		case 'q':
			rotation[2] =  static_cast<GLfloat>(fmod(rotation[2] - 3.0f, 360));
			break;
		case 'w':
			rotation[2] =  static_cast<GLfloat>(fmod(rotation[2] + 3.0f, 360));
			break;
		case 'c':
			newCarPos = carPos - 0.010472f;
			if (wall >= newCarPos)
				return;

			rotation[3] = rotation[3] + 12.0f;
			carPos = newCarPos;
			break;
		case 'v':
			newCarPos = carPos + 0.010472f;
			if (newCarPos >= 0.8f)
				return;

			rotation[3] = rotation[3] - 12.0f;
			carPos = newCarPos;

			break;
		default:
			cout << "Key " << key << " not supported." << endl;
			break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Proyecto 1");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);

	glutKeyboardFunc(getKeyPress);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW error");
		return 1;
	}
	glutMainLoop();
	return 0;
}