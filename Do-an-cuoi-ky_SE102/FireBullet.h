#pragma once
#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

// Fire bullet shooted in 6 direction (left - right)
// 45 degrees: high
// 30 degrees: far
// 60 degrees: low

#define FIRE_BULLET_SPEED_Y 0.03f				// fix (same as the rise speed of piranha plant)

// Basic arctan function to calc the angle based on the Position of bullet and the target
#define FIRE_BULLET_SPEED_X_HIGH 0.03f			// high : arctan(0.03 / 0.03) = 45
												//	=> HIGH (shoot at 45 degrees relative to the horizontal axis)
#define FIRE_BULLET_SPEED_X_FAR 0.05196f		// far  :  arctan(3 * sqrt(3) / 100) = 30 
												//	=> FAR (shoot at 30 degrees relative to the horizontal axis)] 
#define FIRE_BULLET_SPEED_X_LOW 0.017320f		// low  : arctan(sqrt(3) / 100) = 60
												//  => LOW (shoot at 60 degrees relative to the horizontal axis)
#define FIRE_BULLET_BBOX_WIDTH 8
#define FIRE_BULLET_BBOX_HEIGHT 8
//#define FIRE_BULLET_HEIGHT_RISE 24	

#define FIRE_BULLET_ANI 7000
#define FIRE_BULLET_ANI_INACTIVE 7001

#define FIRE_BULLET_STATE_INACTIVE 0
//#define FIRE_BULLET_STATE_RISE 1		// Follow piranha plant
//#define FIRE_BULLET_STATE_DIVE 2 		// Follow piranha plant


#define FIRE_BULLET_STATE_LEFT_SHOOT_HIGH 3
#define FIRE_BULLET_STATE_LEFT_SHOOT_FAR 4
#define FIRE_BULLET_STATE_LEFT_SHOOT_LOW 5
#define FIRE_BULLET_STATE_RIGHT_SHOOT_HIGH 6
#define FIRE_BULLET_STATE_RIGHT_SHOOT_FAR 7
#define FIRE_BULLET_STATE_RIGHT_SHOOT_LOW 8

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
	int IsCollidable() { return 1; }
	int IsBlocking() { return 0; }
};

