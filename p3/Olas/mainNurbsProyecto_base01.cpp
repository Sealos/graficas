// Cubica

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace std;

#define DEF_floorGridScale  1.0
#define DEF_floorGridXSteps 10.0
#define DEF_floorGridZSteps 10.0

float pi = 3.14159f;
float amplitud = 1.f;
float longitud = 3.f;
float velocidad = -0.1f;

static float w = (2 * pi) / longitud;
static float constV = velocidad * ((2 * pi) / longitud);

float decaimiento = 0.f;

float amplitud_ruido = 3.f;
float offset_ruido = 0.8f;
float alturaRuido = 1.f;
float factorTurb = 10.f;

float centroX = 0.0f;
float centroZ = 0.0f;

float centroZParabola = 0.0f;

float factor_cuadratico = 0.0f;

bool pausa = false;
bool desactivarRuido = false;
bool desactivarOla = false;

GLUnurbsObj* theNurb;
GLfloat variableKnots[25] = {
	0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
	7.0f, 8.0f, 9.0f, 10.0f, 11.0f,
	12.0f, 13.0f, 14.0f, 15.0f, 16.0f,
	17.0f, 18.0f, 18.0f, 18.0f, 18.0f
};
GLfloat ctlpoints [21][21][3];

/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static float g3[B + B + 2][3];
static float g2[B + B + 2][2];
static float g1[B + B + 2];
static int start = 1;

static void init(void);


#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )


#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
    t = vec[i] + N;\
    b0 = ((int)t) & BM;\
    b1 = (b0+1) & BM;\
    r0 = t - (int)t;\
    r1 = r0 - 1.f;

float noise2(float vec[2]) {
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	register int i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ;
	u = at2(rx0, ry0);
	q = g2[ b10 ] ;
	v = at2(rx1, ry0);
	a = lerp(sx, u, v);

	q = g2[ b01 ] ;
	u = at2(rx0, ry1);
	q = g2[ b11 ] ;
	v = at2(rx1, ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

static void normalize2(float v[2]) {
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void init_perlin(void) {
	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (float)((rand() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
		{ g2[i][j] = (float)((rand() % (B + B)) - B) / B; }
		normalize2(g2[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
		{ g2[B + i][j] = g2[i][j]; }
		for (j = 0 ; j < 3 ; j++)
		{ g3[B + i][j] = g3[i][j]; }
	}
}

float turbulence(float x, float y, float size) {
	float value = 0.0, initialSize = size;
	float noise[2];

	while (size >= 1) {
		noise[0] = x / size;
		noise[1] = y / size;
		value += noise2(&noise[0]) * size;
		size /= 2.0;
	}

	return (128.0f * value / initialSize);

}

void ejesCoordenada() {

	glLineWidth(2.5);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(0, 10);
	glVertex2f(0, -10);
	glColor3f(0.0, 0.0, 1.0);
	glVertex2f(10, 0);
	glVertex2f(-10, 0);
	glEnd();

	glLineWidth(1.5);
	int i;
	float j;
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	for (i = -10; i <= 10; i++) {
		j = static_cast<float>(i);
		if (i != 0) {
			if ((i % 2) == 0) {
				glVertex2f(j, 0.4f);
				glVertex2f(j, -0.4f);

				glVertex2f(0.4f, j);
				glVertex2f(-0.4f, j);
			}
			else {
				glVertex2f(j, 0.2f);
				glVertex2f(j, -0.2f);

				glVertex2f(0.2f, j);
				glVertex2f(-0.2f, j);

			}
		}
	}

	glEnd();

	glLineWidth(1.0);
}

void changeViewport(int w, int h) {

	if (h == 0) {
		h = 1;
	}
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (GLfloat) w / (GLfloat) h, 1.0, 200.0);
	glMatrixMode(GL_MODELVIEW);

}

void init_surface() {
	for (int i = 0; i < 21; ++i) {
		for (int j = 0; j < 21; ++j) {
			ctlpoints[i][j][0] = i - 10.0f;
			ctlpoints[i][j][1] = 0.0f;
			ctlpoints[i][j][2] = j - 10.0f;
		}
	}
}

void init() {

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	init_surface();

	theNurb = gluNewNurbsRenderer();
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 15.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

	init_perlin();
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			exit(0);
			break;
		case 'a':
			amplitud += 0.1f;
			break;
		case 'z':
			amplitud -= 0.1f;
			break;
		case 's':
			w += 0.1f;
			break;
		case 'x':
			w -= 0.1f;
			break;
		case 'd':
			constV -= 0.1f;
			break;
		case 'c':
			constV += 0.1f;
			break;
		case 'f':
			decaimiento += 0.01f;
			break;
		case 'v':
			decaimiento -= 0.01f;
			break;
		case 'g':
			amplitud_ruido += 0.01f;
			break;
		case 'b':
			amplitud_ruido -= 0.01f;
			break;
		case 'h':
			offset_ruido += 0.01f;
			break;
		case 'n':
			offset_ruido -= 0.01f;
			break;
		case 'j':
			alturaRuido += 0.01f;
			break;
		case 'm':
			alturaRuido -= 0.01f;
			break;
		case 't':
			factorTurb += 1;
			break;
		case 'y':
			factorTurb -= 1;
			break;
		case 'u':

			factor_cuadratico += 0.01f;
			break;
		case 'i':
			factor_cuadratico -= 0.01f;

			break;
		case 'o':
			centroZParabola -= 0.1f;
			break;
		case 'l':
			centroZParabola += 0.1f;
			break;
		case 'q':
			centroX += 0.1f;
			break;
		case 'w':
			centroX -= 0.1f;
			break;
		case 'e':
			centroZ += 0.1f;
			break;
		case 'r':
			centroZ -= 0.1f;
			break;
		case '1':
			pausa = !pausa;
			break;
		case '2':
			desactivarRuido = !desactivarRuido;
			break;
		case '3':
			desactivarOla = !desactivarOla;
			break;
	}
}

void render() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(25.0, 12.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	// Luz y material

	GLfloat mat_diffuse[] = { 0.6, 0.6, 0.9, 1.0 };
GLfloat mat_specular[] = { 0.8, 0.8, 1.0, 1.0 };
 GLfloat mat_shininess[] = {60.0 };
 
 glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
 glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
 glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
 
 
 GLfloat light_ambient[] = { 0.0, 0.0, 0.2, 1.0 };
 GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
 GLfloat light_specular[] = { 0.6, 0.6, 0.6, 1.0 };
  GLfloat light_position[] = { -10.0, -5.0, 0.0, 1.0 };
 
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
 glLightfv(GL_LIGHT0, GL_POSITION, light_position); 



	// Render Grid
	/*
	GLfloat zExtent, xExtent, xLocal, zLocal;
	int loopX, loopZ;
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	glPushMatrix();
	glRotatef(90, 1.0, 0.0, 0.0);
	glColor3f(0.0f, 0.7f, 0.7f);
	glBegin(GL_LINES);
	zExtent = DEF_floorGridScale * DEF_floorGridZSteps;
	for (loopX = -DEF_floorGridXSteps; loopX <= DEF_floorGridXSteps; loopX++) {
		xLocal = DEF_floorGridScale * loopX;
		glVertex3f(xLocal, -zExtent, 0.0);
		glVertex3f(xLocal, zExtent,  0.0);
	}
	xExtent = DEF_floorGridScale * DEF_floorGridXSteps;
	for (loopZ = -DEF_floorGridZSteps; loopZ <= DEF_floorGridZSteps; loopZ++) {
		zLocal = DEF_floorGridScale * loopZ;
		glVertex3f(-xExtent, zLocal, 0.0);
		glVertex3f(xExtent, zLocal, 0.0);
	}
	glEnd();
	ejesCoordenada();
	glPopMatrix();

	glEnable(GL_LIGHTING);
	*/
	// Fin Grid

	//Suaviza las lineas
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);


	glPushMatrix();
	
	gluBeginSurface(theNurb);

	gluNurbsSurface(theNurb,
	                25, variableKnots, 25, variableKnots,
	                21 * 3, 3, &ctlpoints[0][0][0],
	                4, 4, GL_MAP2_VERTEX_3);

	gluEndSurface(theNurb);
	
	glPopMatrix();

	
	/* Muestra los puntos de control */
	/*
	int i, j;
	glPointSize(5.0);
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_POINTS);
	for (i = 0; i < 21; i++) {
		for (j = 0; j < 21; j++) {
			glVertex3f(ctlpoints[i][j][0],  ctlpoints[i][j][1], ctlpoints[i][j][2]);
		}
	}
	
	glEnd();
	*/
	glEnable(GL_LIGHTING);


	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);

	glutSwapBuffers();
}

float tiempo = 0;
void calcularOla() {
	for (int i = 0; i < 21; i++) {
		for (int j = 0; j < 21; j++) {
			float dx = ctlpoints[i][j][0] - centroX;
			float dz = ctlpoints[i][j][2] - centroZ;
			float longitudV = sqrt(dx * dx + dz * dz);
			float ampl = amplitud;
			
			if(longitudV>=1)
				ampl = amplitud* pow((1/(longitudV) ),decaimiento);
			/*else if(longitudV < 1 && longitudV >=0)
				ampl =amplitud*pow((longitudV) ,decaimiento);*/
			if(ampl < 0){
				ampl = 0;
			}
			
			float noise[2];

			noise [0] = ctlpoints[i][j][0] * amplitud_ruido + offset_ruido;
			noise [1] = ctlpoints[i][j][2] * amplitud_ruido + offset_ruido;
			float ruido;
			if (desactivarRuido) {
				ruido = 1;
			}
			else {
				ruido = alturaRuido * 0.005f * turbulence(noise[0], noise[1], factorTurb);
			}
			float ola;
			if (desactivarOla) {
				ola = 1;
			}
			else {
				ola = (ampl * sin((longitudV * w) + (tiempo * constV)));
			}
			
			//cout << "Factor: " <<factor_cuadratico <<" New: " << ctlpoints[i][j][2] << " Old: " << j - 10.0f << endl;

			//y -> altura
			ctlpoints[i][j][1] = ola * ruido + (factor_cuadratico*pow((ctlpoints[i][j][2])+centroZParabola, 2.0f));
			//ctlpoints[i][j][1] = ola * ruido + cosh(ctlpoints[i][j][2]) - (factor_cuadratico*10 + 1);

			//z -> ancho 
			//ctlpoints[i][j][2] = j - 10.0f;
			//float sign = ctlpoints[i][j][2] >= 0 ? 1.f : -1.f;
			//ctlpoints[i][j][2] = sign * abs(j-10) *sqrt((j - 10)*(j - 10) - ctlpoints[i][j][1]*ctlpoints[i][j][1])/10 ;
			//cout << "Y: " << ctlpoints[i][j][1] << "    X: " << ctlpoints[i][j][2] << endl;
		}
	}
}

void animacion(int value) {
	calcularOla();
	if (!pausa) {
		++tiempo;
	}

	glutPostRedisplay();
	glutTimerFunc(10, animacion, 1);

}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960, 540);

	glutCreateWindow("Test Opengl");

	init();
	calcularOla();
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(10, animacion, 1);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}


	glutMainLoop();
	return 0;

}
