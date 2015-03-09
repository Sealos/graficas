#include "stdafx.h"
#include "sierra.h"


Sierra::Sierra(Real movSpeed,SceneNode* nodo,Vector3 rotVec,  Real rotSpeed)
	: vMov(movSpeed),Obstacle(nodo,rotVec,rotSpeed){
		orientacion = nodo ->_getDerivedOrientation();
}


void Sierra::onUpdate(Real dtime){
		Obstacle::onUpdate(dtime);
		Radian angulo = orientacion.getYaw();
		Vector3 traslacion = Vector3(-sin(angulo.valueRadians()),0.0, -cos(angulo.valueRadians()));
		_node -> translate(traslacion *dtime* vMov);
	}
