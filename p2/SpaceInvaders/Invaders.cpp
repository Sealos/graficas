#include "Invaders.h"

class Invaders{
public:

	Invaders(){
		for(int i=0; i < DEF_numberColumns; i++){
			for(int j=0; j<DEF_numberRows; j++){
				invadersAlive[i][j] = true;
			}
		}

		for(int i=0; i <DEF_numberRows ; ++i){
			for(int j=0; j<DEF_numberColumns; ++j){
				if(invadersAlive[j][i] == 1){
					if(i % 2  == 0)
						enemies[j][i] = Invader(leftSidePadding + j*DEF_columnSeparation,upperSidePadding + i*DEF_rowSeparation,i,j,1);
					else{
						//DEF_invaderWidth/2 es el defase
						enemies[j][i] = Invader(leftSidePadding+(DEF_invaderWidth/2)+j*DEF_columnSeparation,upperSidePadding+i*DEF_rowSeparation,i,j,1);
					}
				
				}
			}
		}
		sentidoH =1;
	}

	void draw(){
		for(int i=0; i < DEF_numberColumns; ++i){
			for(int j=0; j< DEF_numberRows; ++j){
				if(invadersAlive[i][j] == 1){
					enemies[i][j].draw();
				}
			}
		}
	}

	float getRightMostX(){
		bool found = false;
		float x= 0.0;
		for(int i = DEF_numberColumns -1; i>=0; i--){
				for(int j =0; j < DEF_numberRows; j++){
					if(invadersAlive[i][j]){
						found = true;
						x = max(x,enemies[i][j].x);
					}
				}
				if(found){
					return x + DEF_invaderWidth/2;
				}
			}
		return x;
	}

	float getLeftMostX(){
		bool found = false;
		float x= 100.0;
		for(int i = 0; i < DEF_numberColumns; ++i){
				for(int j =0; j < DEF_numberRows; j++){
					if(invadersAlive[i][j]){
						found = true;
						x = min(x,enemies[i][j].x);
					}
				}
				if(found){
					return x - DEF_invaderWidth/2;
				}
			}
		return x;
	}

	void updateInvaders(float dx, float dy) {
		for (int i = 0; i < DEF_numberColumns; ++i)
			for (int j = 0; j < DEF_numberRows; ++j)
				enemies[i][j].update(dx, dy);
	}

	bool checkCollisionWithBullet(float pX, float pY) {
		for(int i = DEF_numberColumns -1; i>=0; i--) {
			for(int j =0; j < DEF_numberRows; j++) {
				if(invadersAlive[i][j]) {
						if (enemies[i][j].pointCollision(pX, pY)){
							// matar invader
							return true;
					}
				}
			}
		}

		return false;
	}

	void update(){
		float dx = invaderSpeed*sentidoH;
		updateInvaders(dx, 0);
		float borde;
		if(sentidoH == 1){
			borde = getRightMostX();

			if (borde >= 100.0)
			{
				sentidoH *= -1;
				updateInvaders(0, 5.0);
				invaderSpeed += 0.02f;
			}
		}
		else
		{
			borde = getLeftMostX();
			if (borde <= 0.0)
			{
				sentidoH *= -1;
				updateInvaders(0, 5.0);
				invaderSpeed += 0.02f;
			}
		}
	}
};