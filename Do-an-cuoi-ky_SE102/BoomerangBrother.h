#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Enemy.h"
#include "Textures.h"
#include "Animations.h"

#define BOOMERANG_BROTHER_BBOX_WIDTH 16
#define BOOMERANG_BROTHER_BBOX_HEIGHT 22

#define ID_ANI_BOOMERANG_BROTHER_AIM_RIGHT 13001
#define ID_ANI_BOOMERANG_BROTHER_AIM_LEFT 13002

#define ID_ANI_BOOMERANG_BROTHER_THROW_RIGHT 13003
#define ID_ANI_BOOMERANG_BROTHER_THROW_LEFT 13004

#define BOOMERANG_BROTHER_STATE_AIMING 1
#define BOOMERANG_BROTHER_STATE_THROWING 2

#define BOOMERANG_BROTHER_THROW_DISTANCE 100.0f
#define BOOMERANG_BROTHER_AIM_TIME 500 
#define BOOMERANG_BROTHER_THROW_TIME 1000
#define BOOMERANG_BROTHER_NUMBER_OF_BOOMERANG 2
#define BOOMERANG_BROTHER_WALKING_SPEED 0.05f
#define BOOMERANG_BROTHER_GRAVITY 0.001f

class CBoomerangBrother : public CEnemy
{
private:
	float ay;

	BOOLEAN isAiming; 
	BOOLEAN isThrowing;

	ULONGLONG aimStartTime; 
	ULONGLONG throwStartTime;

	int BoomerangCount;
public:
	CBoomerangBrother(float x, float y) : CEnemy(x, y)
	{
		ay = BOOMERANG_BROTHER_GRAVITY;
		vx = BOOMERANG_BROTHER_WALKING_SPEED;

		isAiming = false;
		isThrowing = false;
		BoomerangCount = 2;
		aimStartTime = 0;
		throwStartTime = 0;
	}
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void SetState(int state);

	//void Fire();

	int IsBlocking() { return 0; } // Boomerang Brother is blocking
	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
};