#pragma once
#include "Enemy.h"
#include "Animation.h"
#include "Animations.h"
#include "debug.h"




class CPiranhaPlant : public CEnemy
{
protected:
	float ax;				// acceleration on x 
	float ay;				// acceleration on y 
	int originalY;
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
public:
	CPiranhaPlant(float x, float y);
	virtual void SetState(int state);
	void SetOriginalPosition(int y) { this->originalY = y; }
};

