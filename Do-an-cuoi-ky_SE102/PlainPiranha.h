#pragma once
#include "Enemy.h"
#include "Animation.h"
#include "Animations.h"
#include "Mario.h"

#define PLAIN_PIRANHA_BBOX_WIDTH 15
#define PLAIN_PIRANHA_BBOX_HEIGHT 21
#define PLAIN_PIRANHA_BBOX_HEIGHT_RISE 19

#define PLAIN_PIRANHA_ANI 6013
#define PLAIN_PIRANHA_ANI_DIE 6006

#define PLAIN_PIRANHA_STATE_HIDE 0
#define PLAIN_PIRANHA_STATE_RISE 1
#define PLAIN_PIRANHA_STATE_IDLE 2
#define PLAIN_PIRANHA_STATE_DIVE 3
#define PLAIN_PIRANHA_STATE_DIE 4

#define PLAIN_PIRANHA_HIDE_TIMEOUT 1000
#define PLAIN_PIRANHA_IDLE_TIMEOUT 2000
#define PLAIN_PIRANHA_DIE_TIMEOUT 205

#define DISTANCE_PLAIN_PIRANHA_NOT_INVOKE 25.5
#define PLAIN_PIRANHA_MOVE_SPEED 0.05f

class CPlainPiranha : public CEnemy
{
protected:
	ULONGLONG stateStartTime;	// time when the current state was started
	float originalY;			// original y pos when starting descending, ascending
	ULONGLONG die_start;
public:
	CPlainPiranha(float x, float y);

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();

	int IsCollidable() {
		if (state == PLAIN_PIRANHA_STATE_DIE || state == PLAIN_PIRANHA_STATE_HIDE)
			return 0;
		return 1;
	};

	int IsBlocking() { return 0; }
	void SetState(int state);
	virtual void Reload();
	CMario* GetPlayer();

	void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
	void OnCollisionWithParaTroopa(LPCOLLISIONEVENT e);
};

