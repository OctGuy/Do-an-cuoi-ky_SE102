#include "PiranhaPlant.h"

CPiranhaPlant::CPiranhaPlant(float x, float y)
{ 
	this->ax = 0;
	this->ay = 0;
}

void CPiranhaPlant::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}