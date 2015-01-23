#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <time.h>

#define GLEW_STATIC

#define DEF_widthWindow 600
#define DEF_heightWindow 600

//Invader definitions al reves
#define DEF_numberColumns 6
#define DEF_numberRows 1
#define DEF_numInvaders DEF_numberRows*DEF_numberColumns

#define DEF_rowSeparation 6.0f
#define DEF_columnSeparation 15.0f
#define DEF_invaderWidth 10.0f
#define DEF_invaderHeight 2.5f

static const float leftSidePadding = 5.0f;
static const float upperSidePadding = 10.0f;
static const float DEGREE = static_cast<float>(3.14159f / 180);

static int points = 0;

static const int timeUFO = 2000;
static const int timeInvaderShoot = 1000;

float invaderSpeed = 0.05f;

using namespace std;

void timerUFO(int);
void timerInvaderShoot(int);

bool existsPlayerBullet = false;

GLvoid* font_style = GLUT_BITMAP_TIMES_ROMAN_24;
//  http://mycodelog.com/tag/printw/
void printw(float x, float y, char* format, ...) {
	va_list args;   //  Variable argument list
	int len;        // String length
	int i;          //  Iterator
	char* text;     // Text

	//  Initialize a variable argument list
	va_start(args, format);

	//  Return the number of characters in the string referenced the list of arguments.
	// _vscprintf doesn't count terminating '\0' (that's why +1)
	len = _vscprintf(format, args) + 1;

	//  Allocate memory for a string of the specified size
	text = static_cast<char*>(malloc(len * sizeof(char)));

	//  Write formatted output using a pointer to the list of arguments
	vsprintf_s(text, len, format, args);

	//  End using variable argument list
	va_end(args);

	//  Specify the raster position for pixel operations.
	glRasterPos2f(x, y);

	//  Draw the characters one by one
	for (i = 0; text[i] != '\0'; i++)
	{ glutBitmapCharacter(font_style, text[i]); }

	//  Free the allocated memory for the string
	free(text);
}



inline pair<int, int> getQuadrant(float x, float y) {
	return pair<int, int>(static_cast<int>(floor(x)) / 10, static_cast<int>(floor(y)) / 10);
}

inline void renderRect(float height, float width, float x, float y) {

	float x_sup = x + width / 2;
	float x_low = x - width / 2;
	float y_sup = y + height / 2;
	float y_low = y - height / 2;
	glBegin(GL_POLYGON);
	{

		glVertex2f(x_low, y_sup);
		glVertex2f(x_low, y_low);
		glColor3f(0.0f, 0.5f, 0.5f);
		glVertex2f(x_sup, y_low);
		glColor3f(0.0f, 0.5f, 0.5f);
		glVertex2f(x_sup, y_sup);
	}
	glEnd();
}

inline void renderCircle(float xx, float yy, float radius) {
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i = i + 10) {
		float degInRad = i * DEGREE;
		float x = xx + cos(degInRad) * radius;
		float y = yy + sin(degInRad) * radius;
		glVertex2f(x, y);
	}
	glEnd();
}

inline float crossProductLinePoint(float x1, float y1, float x2, float y2, float x3, float y3) {
	return (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
};

bool pointCollisionWithRectangle(float pX, float pY, float rX, float rY, float halfWidth, float halfHeight) {
	float recPoints[4][2] = {
		{rX - halfWidth, rY - halfHeight},
		{rX - halfWidth, rY + halfHeight},
		{rX + halfWidth, rY + halfHeight},
		{rX + halfWidth, rY - halfHeight},
	};

	if (crossProductLinePoint(recPoints[0][0], recPoints[0][1], recPoints[1][0], recPoints[1][1], pX, pY) > 0) {
		return false;
	}
	if (crossProductLinePoint(recPoints[1][0], recPoints[1][1], recPoints[2][0], recPoints[2][1], pX, pY) > 0) {
		return false;
	}
	if (crossProductLinePoint(recPoints[2][0], recPoints[2][1], recPoints[3][0], recPoints[3][1], pX, pY) > 0) {
		return false;
	}
	if (crossProductLinePoint(recPoints[3][0], recPoints[3][1], recPoints[0][0], recPoints[0][1], pX, pY) > 0) {
		return false;
	}

	return true;
};

bool isInSameQuadrant(float pX, float pY, float rX, float rY, float halfWidth, float halfHeight) {
	float recPoints[4][2] = {
		{rX - halfWidth, rY - halfHeight},
		{rX - halfWidth, rY + halfHeight},
		{rX + halfWidth, rY + halfHeight},
		{rX + halfWidth, rY - halfHeight},
	};

	pair<int, int> qP = getQuadrant(pX, pY);

	if (qP == getQuadrant(recPoints[0][0], recPoints[0][1])) {
		return true;
	}
	if (qP == getQuadrant(recPoints[1][0], recPoints[1][1])) {
		return true;
	}
	if (qP == getQuadrant(recPoints[2][0], recPoints[2][1])) {
		return true;
	}
	if (qP == getQuadrant(recPoints[3][0], recPoints[3][1])) {
		return true;
	}

	return false;
}

class Bala {
public:
	float x;
	float y;
	float dy;
	int tipo;

	float diferenciales[2];
	bool eraseMe;
	Bala();

	Bala(float startX, float startY, float dy, int tipo);
	void draw();
	void update();
};

vector<Bala> balasInvader = vector<Bala>(10);

class Invader {
public:
	float x;
	float y;
	int vida;
	bool strong;

	Invader() {}

	Invader(float xX, float yY, int vid): vida(vid) {
		x = xX;
		strong = vida > 1;
	}

	inline void draw() {
		glPushMatrix();
		if (vida != 1) {
			glColor3f(1.0f, 0.0f, 0.3f);
		}
		else {
			glColor3f(0.0f, 1.0f, 0.0f);
		}

		renderRect(DEF_invaderHeight, DEF_invaderWidth, x, y);
		glPopMatrix();
	}

	void update(float dx, float dy) {
		x += dx;
		y += dy;
	}

	inline bool vivo() {
		return vida > 0;
	}

	bool pointCollision(float pX, float pY) {
		if (isInSameQuadrant(pX, pY, x, y, DEF_invaderWidth / 2, DEF_invaderHeight / 2)) {
			if (pointCollisionWithRectangle(pX, pY, x, y, DEF_invaderWidth / 2, DEF_invaderHeight / 2)) {
				--vida;

				if (vida == 0) {
					points += strong ? 200 : 100;
				}

				return true;
			}
		}

		return false;
	}

	void shoot() {
		balasInvader.push_back(Bala(x, y + DEF_invaderHeight / 2 + 1.0f, 1.f, 1));
	}
};

class Invaders {
public:
	int sentidoH;
	int alive;

	Invader enemies[DEF_numberColumns][DEF_numberRows];
	Invaders() {
		int vida;
		alive = DEF_numInvaders;
		for (int i = 0; i < DEF_numberRows ; ++i) {
			for (int j = 0; j < DEF_numberColumns; ++j) {
				vida = rand() % 4 == 0 ? 2 : 1;
				if (i % 2  == 0) {
					enemies[j][i] = Invader(leftSidePadding + j * DEF_columnSeparation, upperSidePadding + i * DEF_rowSeparation, vida);
				}
				else {
					//DEF_invaderWidth/2 es el defase
					enemies[j][i] = Invader(leftSidePadding + (DEF_invaderWidth / 2) + j * DEF_columnSeparation, upperSidePadding + i * DEF_rowSeparation, vida);
				}
			}
		}
		sentidoH = 1;
	}

	int countAliveInvaders() {
		alive = 0;
		for (int i = 0; i < DEF_numberColumns; ++i) {
			for (int j = 0; j < DEF_numberRows; ++j) {
				if (enemies[i][j].vivo()) {
					++alive;
				}
			}
		}

		return alive;
	}

	void draw() {
		for (int i = 0; i < DEF_numberColumns; ++i) {
			for (int j = 0; j < DEF_numberRows; ++j) {
				if (enemies[i][j].vivo()) {
					enemies[i][j].draw();
				}
			}
		}
	}

	float getRightMostX() {
		bool found = false;
		float x = 0.0;
		for (int i = DEF_numberColumns - 1; i >= 0; i--) {
			for (int j = 0; j < DEF_numberRows; j++) {
				if (enemies[i][j].vivo()) {
					found = true;
					x = max(x, enemies[i][j].x);
				}
			}
			if (found) {
				return x + DEF_invaderWidth / 2;
			}
		}
		return x;
	}

	float getLeftMostX() {
		bool found = false;
		float x = 100.0;
		for (int i = 0; i < DEF_numberColumns; ++i) {
			for (int j = 0; j < DEF_numberRows; j++) {
				if (enemies[i][j].vivo()) {
					found = true;
					x = min(x, enemies[i][j].x);
				}
			}
			if (found) {
				return x - DEF_invaderWidth / 2;
			}
		}
		return x;
	}

	void updateInvaders(float dx, float dy) {
		for (int i = 0; i < DEF_numberColumns; ++i) {
			for (int j = 0; j < DEF_numberRows; ++j) {
				enemies[i][j].update(dx, dy);
			}
		}
	}

	bool checkCollisionWithPoint(float pX, float pY) {
		for (int i = DEF_numberColumns - 1; i >= 0; i--) {
			for (int j = 0; j < DEF_numberRows; j++) {
				if (enemies[i][j].vivo()) {
					if (enemies[i][j].pointCollision(pX, pY)) {
						// matar invader
						return true;
					}
				}
			}
		}
		return false;
	}

	void update() {
		float dx = invaderSpeed * sentidoH;
		updateInvaders(dx, 0);
		float borde;
		if (sentidoH == 1) {
			borde = getRightMostX();

			if (borde >= 100.0) {
				sentidoH *= -1;
				updateInvaders(0, 2.5);
				invaderSpeed += 0.05f;
			}
		}
		else {
			borde = getLeftMostX();
			if (borde <= 0.0) {
				sentidoH *= -1;
				updateInvaders(0, 2.5);
				invaderSpeed += 0.05f;
			}
		}
	}

	void shoot() {
		int columna = rand() % DEF_numberColumns;
		int limit = columna;
		int fila = rand() % DEF_numberRows;
		int limitF = fila;

		do {
			do {
				if (enemies[columna][fila].vivo()) {
					enemies[columna][fila].shoot();
					return;
				}
				fila = (fila + 1) % DEF_numberRows;
			}
			while ((fila % DEF_numberRows) != limitF);
			columna = (columna + 1) % DEF_numberColumns;
		}
		while ((columna % DEF_numberColumns) != limit);
	}
};

Invaders invaders;

class Barrier {
public:
	float x;
	float y;
	float width;
	float height;
	bool vivo;

	Barrier() {
		x = 0.0f;
		y = 80.f;
		width = 6.0f;
		height = 2.5f;
		vivo = true;
	}

	Barrier(float xX, float yY) {
		x = xX;
		y = yY;
		width = 5.0f;
		height = 2.5f;
		vivo = true;
	}

	void draw() {
		glPushMatrix();
		{
			glColor3f(0.5f, 0.5f, 0.5f);
			renderRect(height, width, x, y);
		}
		glPopMatrix();
	}

	bool pointCollision(float pX, float pY) {

		if (isInSameQuadrant(pX, pY, x, y, width / 2, height / 2)) {
			return pointCollisionWithRectangle(pX, pY, x, y, width / 2, height / 2);
		}
		else {
			return false;
		}
	}

	void update() {
		if (invaders.checkCollisionWithPoint(x + width / 2, y - height / 2)) {
			vivo = false;
			points -= 30;
			return;
		}

		if (invaders.checkCollisionWithPoint(x - width / 2, y - height / 2)) {
			vivo = false;
			points -= 30;
			return;
		}

		if (invaders.checkCollisionWithPoint(x + width / 2, y + height / 2)) {
			vivo = false;
			points -= 30;
			return;
		}

		if (invaders.checkCollisionWithPoint(x - width / 2, y + height / 2)) {
			vivo = false;
			points -= 30;
			return;
		}
	}
};

float bar[8][2] = {
	{12.f, -8.0f}, {18.f, -8.0f},
	{9.f, -4.0f}, {21.f, -4.0f},
	{6.f, 0.f}, {12.f, 0.f,}, {18.f, 0.f}, {24.f, 0.f}
};

class BarrierWall {
public:
	float x;
	float y;

	Barrier barreras[8];

	BarrierWall() {
		x = 0.0;
		y = 81.0;

		for (int i = 0; i < 8; i++) {
			barreras[i] = Barrier(bar[i][0] + x, bar[i][1] + y);
		}
	}

	BarrierWall(float xX, float yY) {
		x = xX;
		y = yY;

		for (int i = 0; i < 8; i++) {
			barreras[i] = Barrier(bar[i][0] + x, bar[i][1] + y);
			barreras[i].vivo = true;
		}
	}

	void draw() {
		for (int i = 0; i < 8; i++) {
			if (barreras[i].vivo) {
				barreras[i].draw();
			}
		}
	}

	bool checkCollisionWithPoint(float pX, float pY) {
		for (int i = 0; i < 8; ++i) {
			if (barreras[i].vivo) {
				if (barreras[i].pointCollision(pX, pY)) {
					// matar barrera
					barreras[i].vivo = false;
					return true;
				}
			}
		}

		return false;
	}


	void update() {
		for (int i = 0; i < 8; ++i) {
			if (barreras[i].vivo) {
				barreras[i].update();

			}
		}
	}
};

class UFO {
public:
	float x;
	float y;
	float dx;
	float width;
	float height;

	UFO() {
		x = 0.0;
		y = 5.0;
		width = 10.0f;
		height = 2.5f;
	}

	UFO(float xX, float yY) {
		x = xX;
		y = yY;
		width = 10.0f;
		height = 2.5f;
	}

	void draw() {
		glPushMatrix();
		{
			glColor3f(0.3f, 0.5f, 0.8f);
			renderRect(height, width, x, y);
		}
		glPopMatrix();
	}

	void update() {
		x += 1.0;
	}

	bool pointCollision(float pX, float pY) {

		if (isInSameQuadrant(pX, pY, x, y, width / 2, height / 2)) {
			return pointCollisionWithRectangle(pX, pY, x, y, width / 2, height / 2);
		}
		else {
			return false;
		}
	}
};

vector<Bala> balas = vector<Bala>(10);

class PlayerShip {
public:

	float x;
	float y;
	float width;
	float height;
	bool keys[2];
	bool vivo;

	PlayerShip() {
		x = 50.0;
		y = 90.0;
		width = 7.0f;
		height = 2.5f;
		keys[0] = false;
		keys[1] = false;
		vivo = true;
	}

	void draw() {
		glPushMatrix();
		{
			glColor3f(0.9f, 0.1f, 0.1f);
			renderRect(height, width, x, y);
			glColor3f(0.9f, 0.1f, 0.1f);
			renderRect(height * 1.5f, width / 4.f, x, y - height / 4.f);
		}
		glPopMatrix();
	}

	void shoot() {
		if (vivo && !existsPlayerBullet) {
			balas.push_back(Bala(x, y - height / 2, -3.f, 0));
			existsPlayerBullet = true;
		}
	}

	void update() {
		//Colisiones con invaders
		if (!vivo) {
			return;
		}

		if (keys[0]) {
			if (x - width / 2 <= 0.0f) {
				return;
			}
			else {
				x -= 1.f;
			}
		}
		else
			if (keys[1]) {
				if (x + width / 2 >= 100.0f) {
					return;
				}
				else {
					x += 1.f;
				}
			}

		if (invaders.checkCollisionWithPoint(x + width / 2.f, y - height / 2.f)) {
			vivo = false;
			return;
		}

		if (invaders.checkCollisionWithPoint(x - width / 2.f, y - height / 2.f)) {
			vivo = false;
			return;
		}

		if (invaders.checkCollisionWithPoint(x + width / 2.f, y + height / 2.f)) {
			vivo = false;
			return;
		}

		if (invaders.checkCollisionWithPoint(x - width / 2.f, y + height / 2.f)) {
			vivo = false;
			return;
		}

		if (invaders.checkCollisionWithPoint(x + width / 8.f, y - height / 2.f - ((height * 1.5f) / 2.f))) {
			vivo = false;
			return;
		}

		if (invaders.checkCollisionWithPoint(x - width / 8.f, y - height / 2.f - ((height * 1.5f) / 2.f))) {
			vivo = false;
			return;
		}
	}

	bool pointCollision(float pX, float pY) {
		if (isInSameQuadrant(pX, pY, x, y, width / 2, height / 2)) {
			return pointCollisionWithRectangle(pX, pY, x, y, width / 2, height / 2);
		}
		else {
			return false;
		}
	}


};

UFO* ufoShip;
PlayerShip player;

BarrierWall barr[3] = { BarrierWall(0.0f, 80.0f), BarrierWall(35.1f, 80.0f), BarrierWall(70.0f, 80.0f)};

bool collisionWall(float pX, float pY) {
	for (int i = 0; i < 3 ; ++i) {
		if (barr[i].checkCollisionWithPoint(pX, pY)) {
			return true;
		}
	}

	return false;
}


bool collisionUFO(float pX, float pY) {
	if (ufoShip && ufoShip ->pointCollision(pX, pY)) {
		delete ufoShip;
		ufoShip = nullptr;
		points += 300;
		glutTimerFunc(timeUFO, timerUFO, 20);
		return true;

	}
	return false;
}

bool collisionPlayer(float pX, float pY) {
	if (player.pointCollision(pX, pY)) {
		player.vivo = false;
		return true;
	}
	else {
		return false;
	}
}

Bala::Bala(float startX, float startY, float dy, int tipoB): x(startX), y(startY), dy(dy), eraseMe(false) {
	diferenciales[0] = 0.6f;
	diferenciales[1] = 0.31519f;

	tipo = tipoB;
};

void Bala::draw() {
	if (tipo == 0) {
		glColor3f(0.8f, 0.1f, 0.1f);
	}
	else {
		glColor3f(0.8f, 0.3f, 0.8f);
	}
	renderCircle(x, y, 0.6f);
}

Bala::Bala() {};

void Bala::update() {
	y += dy;

	float pX = x;
	float pY = y - diferenciales[0];

	if (tipo == 0) {
		if (invaders.checkCollisionWithPoint(pX, pY) || collisionWall(pX, pY) || collisionUFO(pX, pY)) {
			eraseMe = true;
			return;
		}
		pX = x;
		pY = y + diferenciales[0];

		if (invaders.checkCollisionWithPoint(pX, pY) || collisionWall(pX, pY) || collisionUFO(pX, pY)) {
			eraseMe = true;
			return;
		}
		pX = x + diferenciales[0];
		pY = y;

		if (invaders.checkCollisionWithPoint(pX , pY) || collisionWall(pX, pY) || collisionUFO(pX, pY)) {
			eraseMe = true;
			return;
		}
		pX = x - diferenciales[0];
		pY = y;

		if (invaders.checkCollisionWithPoint(pX, pY) || collisionWall(pX, pY) || collisionUFO(pX, pY)) {
			eraseMe = true;
			return;
		}

		pX = x + diferenciales[1];
		pY = y  + diferenciales[1];
		if (invaders.checkCollisionWithPoint(pX, pY) || collisionWall(pX, pY) || collisionUFO(pX, pY)) {
			eraseMe = true;
			return;
		}
		pX = x + diferenciales[1];
		pY = y - diferenciales[1];
		if (invaders.checkCollisionWithPoint(pX , pY) || collisionWall(pX, pY) || collisionUFO(pX, pY)) {
			eraseMe = true;
			return;
		}

		pX = x - diferenciales[1];
		pY = y + diferenciales[1];
		if (invaders.checkCollisionWithPoint(pX, pY) || collisionWall(pX, pY) || collisionUFO(pX, pY)) {
			eraseMe = true;
			return;
		}

		pX = x - diferenciales[1];
		pY = y - diferenciales[1];
		if (invaders.checkCollisionWithPoint(pX, pY) || collisionWall(pX, pY) || collisionUFO(pX, pY)) {
			eraseMe = true;
			return;
		}
	}
	else {
		if (collisionWall(pX, pY) || collisionPlayer(pX, pY)) {
			eraseMe = true;
			return;
		}
		pX = x;
		pY = y + diferenciales[0];


		if (collisionWall(pX, pY) || collisionPlayer(pX, pY)) {
			eraseMe = true;
			return;
		}
		pX = x + diferenciales[0];
		pY = y;

		if (collisionWall(pX, pY) || collisionPlayer(pX, pY)) {
			eraseMe = true;
			return;
		}
		pX = x - diferenciales[0];
		pY = y;

		if (collisionWall(pX, pY) || collisionPlayer(pX, pY)) {
			eraseMe = true;
			return;
		}

		pX = x + diferenciales[1];
		pY = y  + diferenciales[1];
		if (collisionWall(pX, pY) || collisionPlayer(pX, pY)) {
			eraseMe = true;
			return;
		}
		pX = x + diferenciales[1];
		pY = y - diferenciales[1];
		if (collisionWall(pX, pY) || collisionPlayer(pX, pY)) {
			eraseMe = true;
			return;
		}

		pX = x - diferenciales[1];
		pY = y + diferenciales[1];
		if (collisionWall(pX, pY) || collisionPlayer(pX, pY)) {
			eraseMe = true;
			return;
		}

		pX = x - diferenciales[1];
		pY = y - diferenciales[1];
		if (collisionWall(pX, pY) || collisionPlayer(pX, pY)) {
			eraseMe = true;
			return;
		}
	}
};

void changeViewport(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect = (GLfloat)h / (GLfloat)w;

	if (w <= h) {
		glOrtho(0.0, 100.0, 100.0 * aspect, 0.0 * aspect, -1.0, 1.0);
	}
	else {
		glOrtho(0.0 / aspect, 100.0 / aspect, 100.0, 0.0, -1.0, 1.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void cuadriculaColision() {
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	for (float i = 0; i <= 100; i += 10) {
		glVertex2f(0.0, i);
		glVertex2f(100.0, i);
		glVertex2f(i, 0.0);
		glVertex2f(i, 100.0);
	}
	glEnd();
}



void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	//cuadriculaColision();

	invaders.draw();
	barr[0].draw();
	barr[1].draw();
	barr[2].draw();
	for (vector<Bala>::iterator it = balas.begin(); it != balas.end();) {
		if (it->y <= 0.0f || it -> y >= 100.0f || it->eraseMe) {
			if (it->tipo == 0) {
				existsPlayerBullet = false;
			}
			it = balas.erase(it);
		}
		else {
			it->draw();
			++it;
		}
	}

	for (vector<Bala>::iterator it = balasInvader.begin(); it != balasInvader.end();) {
		if (it->y <= 0.0f || it -> y >= 100.0f || it->eraseMe) {
			it = balasInvader.erase(it);
		}
		else {
			it->draw();
			++it;
		}
	}

	if (ufoShip) {
		ufoShip -> draw();
	}

	if (player.vivo) {
		player.draw();
	}
	else {
		glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
		printw(1.0, 50.0, "GAME OVER");
		printw(40.0, 50.0, "GAME OVER");
		printw(75.0, 50.0, "GAME OVER");
		glPopMatrix();
	}


	glutSwapBuffers();
	glutPostRedisplay();
}

void getKeyPress(unsigned char key, int x, int y) {
	switch (key) {
		case 27:  // ESC
			exit(0);
			break;
		case 32:
			player.shoot();
			break;
		case 'a':
			player.keys[0] = true;
			break;
		case 's':
			player.keys[1] = true;
			break;
		default:
			break;
	}
}

void keyboardRelease(unsigned char key, int x, int y) {
	switch (key) {
		case 'a':
			player.keys[0] = false;
			break;
		case 's':
			player.keys[1] = false;
			break;
	}
}

void timerInvaderShoot(int value) {
	invaders.shoot();
	glutTimerFunc(timeInvaderShoot, timerInvaderShoot, 20);
}

void timerUFO(int value) {
	ufoShip = new UFO(0.0, 5.0);
}

void timer(int value) {
	invaders.update();
	player.update();

	barr[0].update();
	barr[1].update();
	barr[2].update();
	glutTimerFunc(50, timer, 20);

	for (vector<Bala>::iterator it = balas.begin(); it != balas.end(); ++it) {
		it->update();
	}

	for (vector<Bala>::iterator it = balasInvader.begin(); it != balasInvader.end(); ++it) {
		it->update();
	}

	if (ufoShip) {
		ufoShip->update();
		if (ufoShip->x - 5.0f >= 100.0) {
			delete ufoShip;
			ufoShip = nullptr;
			glutTimerFunc(timeUFO, timerUFO, 20);
		}
	}
}


int main(int argc, char** argv) {

	srand(static_cast<unsigned int>(time(0)));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(DEF_heightWindow, DEF_widthWindow);
	glutCreateWindow("Space Invaders");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutTimerFunc(50, timer, 20);
	glutTimerFunc(timeInvaderShoot, timerInvaderShoot, 20);
	glutTimerFunc(timeUFO, timerUFO, 20);
	glutKeyboardFunc(getKeyPress);
	glutKeyboardUpFunc(keyboardRelease);

	invaders = Invaders();

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}
	glutMainLoop();
	return 0;
}