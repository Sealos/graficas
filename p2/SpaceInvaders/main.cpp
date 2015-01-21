#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLEW_STATIC

void changeViewport(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect = (GLfloat)h/(GLfloat)w;

	if (w <= h)
		glOrtho(0.0, 100.0, 100.0*aspect,0.0*aspect, -1.0, 1.0);
	else
		glOrtho(0.0/aspect, 100.0/aspect, 100.0, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glBegin(GL_LINES);
	{
		glVertex2f(2.0f, 4.0f);
		glVertex2f(3.0f, 8.0f);
	}
	glEnd();
	glutSwapBuffers();
	glutPostRedisplay();
}

void getKeyPress(unsigned char key, int x, int y)
{

	switch (key)
	{
		case 27:  // ESC
			exit(0);
			break;
		default:
			break;
	}

}

void timer(int value)
{
};

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Space Invaders");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutTimerFunc(100, timer, 20);
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