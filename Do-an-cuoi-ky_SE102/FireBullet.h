#pragma once
#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

// Fire bullet shooted in 6 direction (left - right)
// 45 degrees: high
// 30 degrees: far
// 60 degrees: low

#define FIRE_BULLET_SPEED_Y 0.04f				

// Basic arctan function to calc the angle based on the Position of bullet and the target

#define FIRE_BULLET_SPEED_X_FAR 0.1042f														
#define FIRE_BULLET_SPEED_X_NEAR 0.042745f		
												
#define FIRE_BULLET_BBOX_WIDTH 8
#define FIRE_BULLET_BBOX_HEIGHT 8

#define FIRE_BULLET_ANI 7000
#define FIRE_BULLET_ANI_INACTIVE 7001

#define FIRE_BULLET_STATE_INACTIVE 0
#define FIRE_BULLET_STATE_LEFT_SHOOT_UP_NEAR 3
#define FIRE_BULLET_STATE_LEFT_SHOOT_UP_FAR 4
#define FIRE_BULLET_STATE_LEFT_SHOOT_DOWN_FAR 5
#define FIRE_BULLET_STATE_LEFT_SHOOT_DOWN_NEAR 6
#define FIRE_BULLET_STATE_RIGHT_SHOOT_UP_NEAR 7
#define FIRE_BULLET_STATE_RIGHT_SHOOT_UP_FAR 8
#define FIRE_BULLET_STATE_RIGHT_SHOOT_DOWN_FAR 9
#define FIRE_BULLET_STATE_RIGHT_SHOOT_DOWN_NEAR 10

class CFireBullet : public CGameObject
{
protected:
	int originalX;
	int originalY;
public:
	CFireBullet(float x, float y) : CGameObject(x, y)
	{
		originalX = x;
		originalY = y;
		SetState(FIRE_BULLET_STATE_INACTIVE);
	}

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	virtual void Reload();

	int IsCollidable() { return 1; }
	int IsBlocking() { return 0; }
};

