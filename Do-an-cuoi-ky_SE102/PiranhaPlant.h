#pragma once
#include "Enemy.h"
#include "Animation.h"
#include "Animations.h"
#include "debug.h"

#define PIRANHA_BBOX_WIDTH 16
#define PIRANHA_BBOX_HEIGHT 32
#define PIRANHA_BBOX_HEIGHT_RISE 24

#define PIRANHA_MOVE_SPEED 0.03f

#define PIRANHA_HIDE_TIMEOUT 2500
#define PIRANHA_SNIP_TIMEOUT 2500

#pragma region PIRANHA_STATE_ID
#define PIRANHA_STATE_HIDE 300
#define PIRANHA_STATE_RISE 301
#define PIRANHA_STATE_SNIP 302
#define PIRANHA_STATE_DIVE 303
#pragma endregion

#pragma region PIRANHA_ANIMATION_ID
#define PIRANHA_ANI_LEFT_RISE_DIVE 6000
#define PIRANHA_ANI_RIGHT_RISE_DIVE 6001
#define PIRANHA_ANI_UP_LEFT 6002
#define PIRANHA_ANI_DOWN_LEFT 6003
#define PIRANHA_ANI_UP_RIGHT 6004
#define PIRANHA_ANI_DOWN_RIGHT 6005
#pragma endregion


class CPiranhaPlant : public CEnemy
{
protected:
	ULONGLONG stateStartTime;	// time when the current state was started
	float originalY;			// original y pos when starting descending, ascending

public:
	CPiranhaPlant(float x, float y);

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	int GetSnippingDirection(); // get the direction of the snipping action to mario
	void SetState(int state);
};

