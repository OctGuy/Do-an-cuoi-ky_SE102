#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Sprites.h"
#include "RaccoonTail.h"
#include "Koopa.h"

#define GOOMBA_GRAVITY 0.002f
#define GOOMBA_WALKING_SPEED 0.05f
#define GOOMBA_RISE_SPEED 0.02f
#define GOOMBA_DEFLECT_SPEED 0.4f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 16
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define GOOMBA_OPENED_WING_BBOX_WIDTH 20
#define GOOMBA_OPENED_WING_BBOX_HEIGHT 24
#define GOOMBA_CLOSED_WING_BBOX_WIDTH 20
#define GOOMBA_CLOSED_WING_BBOX_HEIGHT 19

#define GOOMBA_DIE_TIMEOUT 500
#define GOOMBA_DIE_REVERSE_TIMEOUT 1500

#pragma region BASE_GOOMBA
#define GOOMBA_STATE_RISE 100
#define GOOMBA_STATE_WALKING 110
#define GOOMBA_STATE_DIE 200
#define GOOMBA_STATE_DIE_REVERSE 201
#pragma endregion

#pragma region WINGED_GOOMBA
#define GOOMBA_WING_STATE_FLY 202
#define GOOMBA_WING_STATE_WALKING 203
#define GOOMBA_WING_STATE_HIGH_JUMP 204
#define GOOMBA_WING_STATE_LOW_JUMP 205
#define GOOMBA_WING_STATE_DIE 206
#define GOOMBA_WING_STATE_DIE_REVERSE 207
#pragma endregion

#define GOOMBA_TEXTURE_IDLE 31001

#define ID_ANI_GOOMBA_WALKING 5000
#define ID_ANI_GOOMBA_DIE 5001
#define ID_ANI_GOOMBA_DIE_REVERSE 5002

#define BASE_GOOMBA_TYPE 1
#define WINGED_GOOMBA_TYPE 2

class CGoomba : public CGameObject
{
protected:
	float ax;				
	float ay; 

	int originalY;
	int type;

	//Need to create an enemy class and implement this instead of just goomba
	int currentAniId = GOOMBA_TEXTURE_IDLE; 

	ULONGLONG die_start;

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();

	virtual int IsCollidable() { 
		if (state == GOOMBA_STATE_DIE_REVERSE
			|| state == GOOMBA_STATE_DIE)
			return 0;
		return 1;
	};
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithTail(LPCOLLISIONEVENT e);
public: 	
	CGoomba(float x, float y, int type);
	virtual void SetState(int state);
};