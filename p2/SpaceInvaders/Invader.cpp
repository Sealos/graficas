#include "Invader.h"


class Invader{
public:
	float x;
	float y;
	//Posicion en el arreglo de enemigos. Necesario para cuando ocurra una colision se sepa cual fue que lo ocasiono
	int idX;
	int idY;

	int vida;

	Invader(){}

	Invader(float xX, float yY, int i, int j, int vid):vida(vid){
		x = xX;
		y = yY;
		idX = j;
		idY =i;
	}

	inline void draw(){
		renderRect(DEF_invaderHeight,DEF_invaderWidth,x,y);
	}

	void update(float dx, float dy) {
		x += dx;
		y += dy;
	}

	bool pointCollision(float pX, float pY) {

		if (isInSameQuadrant(pX, pY, x, y, DEF_invaderWidth/2, DEF_invaderHeight/2)) 
			return pointCollisionWithRectangle(pX, pY, x, y, DEF_invaderWidth/2, DEF_invaderHeight/2);
		else
			return false;

	}

};