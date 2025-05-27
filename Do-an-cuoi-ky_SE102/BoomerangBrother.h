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

#define ID_ANI_BOOMERANG_BROTHER_DIE 13009

#define BOOMERANG_BROTHER_STATE_AIMING 1
#define BOOMERANG_BROTHER_STATE_THROWING 2
#define BOOMERANG_BROTHER_STATE_DIE 4

#define BOOMERANG_BROTHER_THROW_DISTANCE 100.0f
#define BOOMERANG_BROTHER_AIM_TIME 500 
#define BOOMERANG_BROTHER_THROW_TIME 1000
#define BOOMERANG_BROTHER_NUMBER_OF_BOOMERANG 2
#define BOOMERANG_BROTHER_WALKING_SPEED 0.03f
#define BOOMERANG_BROTHER_GRAVITY 0.001f
#define BOOMERANG_BROTHER_PATROL_DISTANCE 20.0f
#define BOOMERANG_BROTHER_WAIT_TIME 300
#define BOOMERANG_BROTHER_DIE_TIMEOUT 500

class CBoomerangBrother : public CEnemy
{
private:
	float ay;

	float originalX; 

	BOOLEAN isAiming; 
	BOOLEAN isThrowing;

	ULONGLONG aimStartTime; 
	ULONGLONG throwStartTime;

	ULONGLONG waitStartTime; 
	ULONGLONG die_start;

	int BoomerangCount;

	LPGAMEOBJECT boomerang;

	void OnCollisionWithBoomerang(LPCOLLISIONEVENT e);
public:
	CBoomerangBrother(float x, float y) : CEnemy(x, y)
	{
		originalX = x;

		ay = BOOMERANG_BROTHER_GRAVITY;
		vx = BOOMERANG_BROTHER_WALKING_SPEED;

		isAiming = false;
		isThrowing = false;
		BoomerangCount = BOOMERANG_BROTHER_NUMBER_OF_BOOMERANG;
		aimStartTime = 0;
		throwStartTime = 0;

		boomerang = nullptr;
	}
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void SetState(int state);
	void Reload() override;

	void Throw();

	int IsBlocking() { return 0; }
	int IsCollidable() override
	{ 
		if (state == BOOMERANG_BROTHER_STATE_DIE)
			return 0;
		return 1;
	} 
	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
};