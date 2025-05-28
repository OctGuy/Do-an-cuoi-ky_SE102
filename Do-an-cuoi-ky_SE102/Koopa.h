#pragma once
#include "Enemy.h"
#include "Animation.h"
#include "Animations.h"
#include "Platform.h"
#include "BoxPlatform.h"
#include "Brick.h"
#include "Mario.h"
#include "PlayScene.h"
#include "PiranhaPlant.h"

#define KOOPA_ANI_WALKING_LEFT 8000
#define KOOPA_ANI_WALKING_RIGHT 8001
#define KOOPA_ANI_SHELL_IDLE 8002
#define KOOPA_ANI_SHELL_MOVE 8003
#define KOOPA_ANI_SHELL_SHAKING 8004
#define KOOPA_ANI_SHELL_REVERSE_IDLE 8005
#define KOOPA_ANI_SHELL_REVERSE_MOVE 8006
#define KOOPA_ANI_SHELL_REVERSE_SHAKING 8007
#define KOOPA_ANI_DIE 8007 
#define KOOPA_ANI_FLY 8018

#define KOOPA_GRAVITY 0.001f
#define KOOPA_WALKING_SPEED 0.03f
#define KOOPA_SHELL_SPEED 0.15f
#define KOOPA_DEFLECT_SPEED 0.3f
#define KOOPA_FLY_HEIGHT 112.0f
#define KOOPA_FLY_SPEED 0.03f

#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 26

#define KOOPA_SHELL_BBOX_WIDTH 14
#define KOOPA_SHELL_BBOX_HEIGHT 14

#define KOOPA_SHELL_DURATION 5000
#define KOOPA_SHELL_SHAKING_DURATION 2000
#define KOOPA_DIE_DURATION 500

#define KOOPA_STATE_WALKING_LEFT 0
#define KOOPA_STATE_WALKING_RIGHT 1
#define KOOPA_STATE_SHELL_IDLE 2
#define KOOPA_STATE_SHELL_MOVE 3
#define KOOPA_STATE_SHELL_SHAKING 4
#define KOOPA_STATE_SHELL_REVERSE_IDLE 5
#define KOOPA_STATE_SHELL_REVERSE_MOVE 6
#define KOOPA_STATE_SHELL_REVERSE_SHAKING 7
#define KOOPA_STATE_DIE 8
#define KOOPA_STATE_SHELL_REVERSE_JUMP	9
#define KOOPA_STATE_FLY 10

#define FLYING_KOOPA_ORIGINAL_X 1868.0f
#define FLYING_KOOPA_ORIGINAL_Y 293.0f

class CKoopa : public CEnemy
{
protected:
	float ax;
	float ay;

	float flyUpperY;
	float flyLowerY;

	bool isFlyingUp;

	ULONGLONG stateShellStart;
	ULONGLONG stateShakingStart;
	ULONGLONG die_start;

	bool isHeld;
	BOOLEAN isInWall;
public:
	CKoopa(float x, float y) : CEnemy(x, y)
	{
		this->ax = 0;
		this->ay = KOOPA_GRAVITY;

		flyUpperY = 0;
		flyLowerY = 0;

		if (x == FLYING_KOOPA_ORIGINAL_X
			&& y == FLYING_KOOPA_ORIGINAL_Y)
			SetState(KOOPA_STATE_FLY);
		else SetState(KOOPA_STATE_WALKING_LEFT);

		stateShellStart = -1;
		stateShakingStart = -1;
		die_start = -1;

		isHeld = false;
		isInWall = false;
		isFlyingUp = false;
	}

	void SetIsHeld(bool isHeld) { this->isHeld = isHeld; }
	bool GetIsHeld() { return isHeld; }
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	virtual void Reload();
	int IsCollidable() { return (state != KOOPA_STATE_DIE); }

	int IsBlocking() { return 0; }

	BOOLEAN IsInWall() { return isInWall; }

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
	CMario* GetPlayer();
	bool IsPlatformEdge(float checkDistance, vector<LPGAMEOBJECT>& possibleGrounds);
	void OnCollisionWithBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
	void OnCollisionWithShinyBrick(LPCOLLISIONEVENT e);
};

