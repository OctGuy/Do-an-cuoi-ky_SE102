#pragma once
#include "Enemy.h"
#include "Mario.h"

#define GOOMBA_WING_STATE_FLY 202
#define GOOMBA_WING_STATE_WALKING 203
#define GOOMBA_WING_STATE_BOUNCE 204
#define GOOMBA_WING_STATE_DIE 205
#define GOOMBA_WING_STATE_DIE_REVERSE 206
#define GOOMBA_WING_STATE_TRACKING_MARIO 207

#define GOOMBA_BASE_BBOX_WIDTH 16
#define GOOMBA_BASE_BBOX_HEIGHT 16
#define GOOMBA_WING_BBOX_WIDTH 20
#define GOOMBA_WING_BBOX_HEIGHT 19
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define GOOMBA_WING_DIE_TIMEOUT 500
#define GOOMBA_WING_DIE_REVERSE_TIMEOUT 1000
#define GOOMBA_TRACKING_TIMEOUT 1000

#define GOOMBA_WING_GRAVITY 0.001f
#define GOOMBA_WING_WALKING_SPEED 0.04f
#define GOOMBA_WING_BOUNCE_SPEED 0.125f
#define GOOMBA_WING_FLY_UP_SPEED 0.25f
#define GOOMBA_WING_DEFLECT_SPEED 0.4f

#define GOOMBA_WING_ANI_FLY 5003
#define GOOMBA_WING_ANI_OPEN_WING 5004
#define GOOMBA_WING_ANI_CLOSE_WING 5005
#define GOOMBA_WING_ANI_WALKING 5006
#define GOOMBA_WING_ANI_DIE 5007
#define GOOMBA_WING_ANI_DIE_REVERSE 5008
#define GOOMBA_WING_ANI_TRACKING_MARIO 5009

class CWingedGoomba : public CGameObject
{
protected:
	float ax;
	float ay;
	ULONGLONG die_start;
	ULONGLONG tracking_start;
	bool isOnPlatform;
	bool wasOnPlatform;
	int bounceCount;
public:
	CWingedGoomba(float x, float y);

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();

	int IsCollidable() {
		if (state == GOOMBA_WING_STATE_DIE_REVERSE
			|| state == GOOMBA_WING_STATE_DIE)
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
	void TrackingMario();
	void Reload();
};