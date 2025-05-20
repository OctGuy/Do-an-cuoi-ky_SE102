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
#define KOOPA_ANI_DIE 8007 // same as shell move

#define KOOPA_GRAVITY 0.001f
#define KOOPA_WALKING_SPEED 0.03f
#define KOOPA_SHELL_SPEED 0.15f
#define KOOPA_DEFLECT_SPEED 0.3f

#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 26

#define KOOPA_SHELL_BBOX_WIDTH 16
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


class CKoopa : public CEnemy
{
protected:
	float ax;
	float ay;

	ULONGLONG stateShellStart;
	ULONGLONG stateShakingStart;
	ULONGLONG die_start;

	LPGAMEOBJECT platform;

	bool isHeld;
	BOOLEAN isInWall;
public:
	CKoopa(float x, float y) : CEnemy(x, y)
	{
		this->ax = 0;
		this->ay = KOOPA_GRAVITY;
		SetState(KOOPA_STATE_WALKING_LEFT);
		stateShellStart = -1;
		stateShakingStart = -1;
		die_start = -1;
		isHeld = false;
		isInWall = false;
		platform = NULL;
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
	bool IsOnPlatform();
	CMario* GetPlayer();

	void OnCollisionWithBrick(LPCOLLISIONEVENT e);
};

