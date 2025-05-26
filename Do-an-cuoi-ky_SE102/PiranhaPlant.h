#pragma once
#include "Enemy.h"
#include "Animation.h"
#include "Animations.h"
#include "debug.h"
#include "FireBullet.h"
#include "Mario.h"
#include <utility>

#pragma region SNIP_RANGE
#define FAR_POINT_LEFT_RIGHT 119
#define NEAR_POINT_MIN_LEFT 22
#define NEAR_POINT_MAX_LEFT 23
#define NEAR_POINT_MIN_RIGHT 23
#define NEAR_POINT_MAX_RIGHT 24
#pragma endregion

#define LIMIT_RANGE_TO_SHOOT 79
#define DISTANCE_PIRANHA_NOT_INVOKE 25.5

#define PIRANHA_BBOX_WIDTH 16
#define PIRANHA_BBOX_HEIGHT 26
#define PIRANHA_BBOX_HEIGHT_RISE 24

#define GREEN_PIRANHA_BBOX_WIDTH 16
#define GREEN_PIRANHA_BBOX_HEIGHT 23
#define GREEN_PIRANHA_BBOX_HEIGHT_RISE 20

#define PIRANHA_MOVE_SPEED 0.03f

#define PIRANHA_HIDE_TIMEOUT 2500
#define PIRANHA_SNIP_TIMEOUT 3500
#define PIRANHA_WAIT_FOR_SHOOT_TIMEOUT 2000
#define PIRANHA_DIE_TIMEOUT 205

#pragma region PIRANHA_STATE_ID
#define PIRANHA_STATE_HIDE 300
#define PIRANHA_STATE_RISE 301
#define PIRANHA_STATE_SNIP 302
#define PIRANHA_STATE_DIVE 303
#define PIRANHA_STATE_DIE 304
#pragma endregion

#pragma region PIRANHA_ANIMATION_ID
#define PIRANHA_ANI_LEFT_RISE_DIVE 6000
#define PIRANHA_ANI_RIGHT_RISE_DIVE 6001
#define PIRANHA_ANI_UP_LEFT 6002
#define PIRANHA_ANI_DOWN_LEFT 6003
#define PIRANHA_ANI_UP_RIGHT 6004
#define PIRANHA_ANI_DOWN_RIGHT 6005
#define PIRANHA_ANI_DIE 6006

#define GREEN_PIRANHA_ANI_LEFT_RISE_DIVE 6007
#define GREEN_PIRANHA_ANI_RIGHT_RISE_DIVE 6008
#define GREEN_PIRANHA_ANI_UP_LEFT 6009
#define GREEN_PIRANHA_ANI_DOWN_LEFT 6010
#define GREEN_PIRANHA_ANI_UP_RIGHT 6011
#define GREEN_PIRANHA_ANI_DOWN_RIGHT 6012
#pragma endregion

#pragma region PIRANHA_RANGE
#define IN_RANGE_OF_LEFT 100
#define IN_RANGE_OF_RIGHT 200
#pragma endregion

#define GREEN_PIRANHA_ORIGINAL_X 1863.0f
#define GREEN_PIRANHA_ORIGINAL_Y 367.0f

class CPiranhaPlant : public CEnemy
{
protected:
	ULONGLONG stateStartTime;	// time when the current state was started
	float originalX;			
	float originalY;			// original y pos when starting descending, ascending
	bool isShooting;			// check if the piranha plant is shooting
	ULONGLONG die_start;
	bool isRed;
public:
	CPiranhaPlant(float x, float y);

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();

	int IsCollidable() {
		if (state == PIRANHA_STATE_DIE || state == PIRANHA_STATE_HIDE)
			return 0;
		return 1;
	};

	int IsBlocking() { return 0; }
	int GetSnippingDirection(); // get the direction of the snipping action to mario
	void SetState(int state);
	void Shoot(int direction);
	bool IsTargetInRange();	
	virtual void Reload();

	CMario* GetPlayer();

	void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
	void OnCollisionWithParaTroopa(LPCOLLISIONEVENT e);
};

