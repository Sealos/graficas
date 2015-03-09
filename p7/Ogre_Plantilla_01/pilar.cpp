#include "stdafx.h"
#include "pilar.h"

Pilar::Pilar(Real vSpeed, SceneNode* nodo, Vector3 orient, Real rotS)
	:vMov(vSpeed), Obstacle(nodo,orient,rotS){

}

void Pilar::onUpdate(Real dtime){
	Obstacle::onUpdate(dtime);

	Vector3 traslacion = Vector3(0.0,-1.0, 0.0);
	_node -> translate(traslacion *dtime* vMov);
}