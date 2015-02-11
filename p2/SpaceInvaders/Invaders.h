
#include "Invader.h";
#pragma once

static const float leftSidePadding = 5.0f;
static const float upperSidePadding = 10.0f;

class Invaders
{
public:
	int sentidoH;
	bool invadersAlive[DEF_numberColumns][DEF_numberRows];
	Invader enemies[DEF_numberColumns][DEF_numberRows];
	Invaders(void);
	~Invaders(void);
};

