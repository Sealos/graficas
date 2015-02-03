// Cubica

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace std;

#define DEF_floorGridScale	1.0
#define DEF_floorGridXSteps	10.0
#define DEF_floorGridZSteps	10.0

float pi = 3.14159f;
float amplitud = 2.f;
float longitud = 3.f;
float velocidad = -0.1f;

float w = (2*pi)/longitud;
float constV = velocidad *((2*pi)/longitud);

float decaimiento = amplitud;

float amplitud_ruido = 3.f;
float offset_ruido = 0.8f;
float alturaRuido = 1.f;
float factorTurb = 10.f;


GLUnurbsObj *theNurb;
GLfloat variableKnots[25] = {
	0.0f,0.0f,0.0f,0.0f,1.0f,
	2.0f,3.0f,4.0f,5.0f,6.0f,
	7.0f,8.0f,9.0f,10.0f,11.0f,
	12.0f,13.0f,14.0f,15.0f,16.0f,
	17.0f,18.0f,18.0f,18.0f,18.0f
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


float noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	register int i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

static void normalize2(float v[2])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}
static void init_perlin(void)
{
	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (float)((rand() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((rand() % (B + B)) - B) / B;
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
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}



float turbulence(float x, float y, float size)
{
	float value = 0.0, initialSize = size;
	float noise[2];

	while(size >= 1)
	{
		noise[0] = x/size;
		noise[1] = y/size;
		value += noise2(&noise[0]) * size;
		size /= 2.0;
	}

	return(128.0f * value / initialSize);

}

void ejesCoordenada() {

	glLineWidth(2.5);
	glBegin(GL_LINES);
	glColor3f(1.0,0.0,0.0);
	glVertex2f(0,10);
	glVertex2f(0,-10);
	glColor3f(0.0,0.0,1.0);
	glVertex2f(10,0);
	glVertex2f(-10,0);
	glEnd();

	glLineWidth(1.5);
	int i;
	float j;
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);
	for(i = -10; i <=10; i++){
		j = static_cast<float>(i);
		if (i!=0) {		
			if ((i%2)==0){
				glVertex2f(j,0.4f);
				glVertex2f(j,-0.4f);

				glVertex2f(0.4f,j);
				glVertex2f(-0.4f,j);
			}else{
				glVertex2f(j,0.2f);
				glVertex2f(j,-0.2f);

				glVertex2f(0.2f,j);
				glVertex2f(-0.2f,j);

			}
		}
	}

	glEnd();

	glLineWidth(1.0);
}

void changeViewport(int w, int h) {

	float aspectratio;

	if (h==0)
		h=1;
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(30, (GLfloat) w/(GLfloat) h, 1.0, 200.0);
	glMatrixMode (GL_MODELVIEW);

}

void init_surface() {

	for(int i = 0; i <21; ++i){
		for(int j = 0; j<21; ++j){
			ctlpoints[i][j][0] = i-10.0f;
			ctlpoints[i][j][1] = 0.0f;
			ctlpoints[i][j][2] = j-10.0f;
		}
	}

}

void init(){



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



void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:             
		exit (0);
		break;

	}
}



void render(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat zExtent, xExtent, xLocal, zLocal;
	int loopX, loopZ;

	glLoadIdentity ();                       
	gluLookAt (25.0, 12.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	// Luz y material

	GLfloat mat_diffuse[] = { 0.6f, 0.6f, 0.9f, 1.0f };
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = {10.f };

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);


	GLfloat light_ambient[] = { 0.0f, 0.0f, 0.2f, 1.0f };
	GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light_position[] = { -10.0f, -5.0f, 0.0f, 1.0f };


	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);   



	// Render Grid 
	/*
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	glPushMatrix();
	glRotatef(90,1.0,0.0,0.0);
	glColor3f( 0.0, 0.7, 0.7 );
	glBegin( GL_LINES );
	zExtent = DEF_floorGridScale * DEF_floorGridZSteps;
	for(loopX = -DEF_floorGridXSteps; loopX <= DEF_floorGridXSteps; loopX++ )
	{
	xLocal = DEF_floorGridScale * loopX;
	glVertex3f( xLocal, -zExtent, 0.0 );
	glVertex3f( xLocal, zExtent,  0.0 );
	}
	xExtent = DEF_floorGridScale * DEF_floorGridXSteps;
	for(loopZ = -DEF_floorGridZSteps; loopZ <= DEF_floorGridZSteps; loopZ++ )
	{
	zLocal = DEF_floorGridScale * loopZ;
	glVertex3f( -xExtent, zLocal, 0.0 );
	glVertex3f(  xExtent, zLocal, 0.0 );
	}
	glEnd();
	ejesCoordenada();
	glPopMatrix();

	glEnable(GL_LIGHTING);

	// Fin Grid
	*/


	//Suaviza las lineas
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_LINE_SMOOTH );	


	glPushMatrix();

	gluBeginSurface(theNurb);

	gluNurbsSurface(theNurb, 
		25, variableKnots, 25, variableKnots,
		21 * 3, 3, &ctlpoints[0][0][0], 
		4, 4, GL_MAP2_VERTEX_3);
	/*

	No cambien los numeros de la funcion, solo deben de poner los nombres de las variables correspondiente.

	*/


	gluEndSurface(theNurb);

	glPopMatrix();


	/* Muestra los puntos de control */
	/*
	int i,j;
	glPointSize(5.0);
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_POINTS);
	for (i = 0; i <21; i++) {
	for (j = 0; j < 21; j++) {
	glVertex3f(ctlpoints[i][j][0], 	ctlpoints[i][j][1], ctlpoints[i][j][2]);
	}
	}
	glEnd();
	glEnable(GL_LIGHTING);


	*/
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);

	glutSwapBuffers();
}
float tiempo=0;
void calcularOla(){
	for(int i = 0; i <21; i++){
		for(int j = 0; j < 21; j++){
			float longitudV = sqrt((ctlpoints[i][j][0]*ctlpoints[i][j][0]) +(ctlpoints[i][j][2]*ctlpoints[i][j][2]) );
			float ampl = amplitud;
			if(longitudV!=0)
				ampl = amplitud/(longitudV*1.5f);

			if(ampl < 0){
				ampl = 0;
			}
			float noise[2];
			noise [0] = ctlpoints[i][j][0] * amplitud_ruido + offset_ruido;
			noise [1] = ctlpoints[i][j][2] * amplitud_ruido + offset_ruido;

			float ruido = alturaRuido*0.005f*turbulence(noise[0],noise[1],factorTurb);
			ctlpoints[i][j][1] = (ampl * sin((longitudV * w) + (tiempo*constV))) + ruido;
		}
	}
}

void animacion(int value) {
	calcularOla();
	++tiempo;

	glutPostRedisplay();
	glutTimerFunc(10,animacion,1);

}

int main (int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960,540);

	glutCreateWindow("Test Opengl");

	init ();

	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutKeyboardFunc (Keyboard);
	calcularOla();
	glutTimerFunc(10,animacion,1);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}


	glutMainLoop();
	return 0;

}
