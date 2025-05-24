#pragma once
#include "Enemy.h"
#include "Animation.h"
#include "Animations.h"
#include "Platform.h"
#include "BoxPlatform.h"
#include "Brick.h"
#include "Mario.h"
#include "PlayScene.h"
#include "ShinyBrick.h"
#include "PiranhaPlant.h"

#define PARATROOPA_ANI_WALKING_LEFT 8008
#define PARATROOPA_ANI_WALKING_RIGHT 8009
#define PARATROOPA_ANI_BOUNCE_LEFT 8010
#define PARATROOPA_ANI_BOUNCE_RIGHT 8011
#define PARATROOPA_ANI_SHELL_IDLE 8012
#define PARATROOPA_ANI_SHELL_MOVE 8013
#define PARATROOPA_ANI_SHELL_SHAKING 8014
#define PARATROOPA_ANI_SHELL_REVERSE_IDLE 8015
#define PARATROOPA_ANI_SHELL_REVERSE_MOVE 8016
#define PARATROOPA_ANI_SHELL_REVERSE_SHAKING 8017
#define PARATROOPA_ANI_DIE 8016

#define PARATROOPA_GRAVITY 0.001f
#define PARATROOPA_WALKING_SPEED 0.03f
#define PARATROOPA_SHELL_SPEED 0.15f
#define PARATROOPA_DEFLECT_SPEED 0.3f
#define PARATROOPA_BOUNCE_SPEED 0.3f

#define PARATROOPA_BBOX_WIDTH 16
#define PARATROOPA_BBOX_HEIGHT 26

#define PARATROOPA_SHELL_BBOX_WIDTH 16
#define PARATROOPA_SHELL_BBOX_HEIGHT 16

#define PARATROOPA_SHELL_DURATION 5000
#define PARATROOPA_SHELL_SHAKING_DURATION 2000
#define PARATROOPA_DIE_DURATION 500

#define PARATROOPA_STATE_WALKING_LEFT 10
#define PARATROOPA_STATE_WALKING_RIGHT 11
#define PARATROOPA_STATE_BOUNCE_LEFT 12
#define PARATROOPA_STATE_BOUNCE_RIGHT 13
#define PARATROOPA_STATE_SHELL_IDLE 14
#define PARATROOPA_STATE_SHELL_MOVE 15
#define PARATROOPA_STATE_SHELL_SHAKING 16
#define PARATROOPA_STATE_SHELL_REVERSE_IDLE 17
#define PARATROOPA_STATE_SHELL_REVERSE_MOVE 18
#define PARATROOPA_STATE_SHELL_REVERSE_SHAKING 19
#define PARATROOPA_STATE_DIE 20
#define PARATROOPA_STATE_SHELL_REVERSE_JUMP 21

class CParaTroopa : public CEnemy
{
protected:
	float ax;
	float ay;

	ULONGLONG stateShellStart;
	ULONGLONG stateShakingStart;
	ULONGLONG die_start;

	bool isHeld;
	BOOLEAN isInWall;
public:
	CParaTroopa(float x, float y) : CEnemy(x, y)
	{
		this->ax = 0;
		this->ay = PARATROOPA_GRAVITY;
		SetState(PARATROOPA_STATE_BOUNCE_LEFT);
		stateShellStart = -1;
		stateShakingStart = -1;
		die_start = -1;
		isHeld = false;
		isInWall = false;
	}

	void SetIsHeld(bool isHeld) { this->isHeld = isHeld; }
	bool GetIsHeld() { return isHeld; }
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	virtual void Reload();
	int IsCollidable() { return (state != PARATROOPA_STATE_DIE); }

	int IsBlocking() { return 0; }

	BOOLEAN IsInWall() { return isInWall; }

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
	CMario* GetPlayer();
	void OnCollisionWithBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithShinyBrick(LPCOLLISIONEVENT e);
};

