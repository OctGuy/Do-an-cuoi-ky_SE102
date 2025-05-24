#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Sprites.h"
#include "RaccoonTail.h"
#include "Koopa.h"
#include "ParaTroopa.h"

#define GOOMBA_GRAVITY 0.002f
#define GOOMBA_WALKING_SPEED 0.05f
#define GOOMBA_RISE_SPEED 0.02f
#define GOOMBA_DEFLECT_SPEED 0.4f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 16
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define GOOMBA_DIE_TIMEOUT 500
#define GOOMBA_DIE_REVERSE_TIMEOUT 500

#pragma region BASE_GOOMBA
#define GOOMBA_STATE_RISE 100
#define GOOMBA_STATE_WALKING 110
#define GOOMBA_STATE_DIE 200
#define GOOMBA_STATE_DIE_REVERSE 201
#pragma endregion

#define GOOMBA_TEXTURE_IDLE 31001

#define ID_ANI_GOOMBA_WALKING 5000
#define ID_ANI_GOOMBA_DIE 5001
#define ID_ANI_GOOMBA_DIE_REVERSE 5002

class CGoomba : public CGameObject
{
protected:
	float ax;
	float ay;

	int originalY;

	//Need to create an enemy class and implement this instead of just goomba
	int currentAniId = GOOMBA_TEXTURE_IDLE;

	ULONGLONG die_start;
public:
	CGoomba(float x, float y);

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	virtual void Reload();

	int IsCollidable() {
		if (state == GOOMBA_STATE_DIE_REVERSE
			|| state == GOOMBA_STATE_DIE)
			return 0;
		return 1;
	};
	int IsBlocking() { return 0; }
	void OnNoCollision(DWORD dt);

	void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
	void OnCollisionWithParaTroopa(LPCOLLISIONEVENT e);
	void SetState(int state);
	CMario* GetPlayer();
};