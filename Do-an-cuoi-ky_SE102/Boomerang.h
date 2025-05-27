#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "Textures.h"
#include "Animations.h"

#define BOOMERANG_BBOX_WIDTH 16
#define BOOMERANG_BBOX_HEIGHT 16

#define BOOMERANG_STATE_THROWN 1
#define BOOMERANG_STATE_HELD 2

#define ID_ANI_BOOMERANG_FLYING_RIGHT 13005
#define ID_ANI_BOOMERANG_FLYING_LEFT 13006
#define ID_ANI_BOOMERANG_HELD_RIGHT 13007
#define ID_ANI_BOOMERANG_HELD_LEFT 13008

#define BOOMERANG_FLYING_SPEED 0.15f
#define BOOMERANG_FLYING_SPEED_Y 0.07f

class CBoomerang : public CGameObject
{
private:
	float ax;
	float ay;

	BOOLEAN isThrown;
	BOOLEAN isReturning;
public:
	CBoomerang(float x, float y) : CGameObject(x, y)
	{
		ax = 0.0f;
		ay = 0.0f;
		isThrown = false;
		isReturning = false;
	}
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	int IsBlocking() { return 0; }
	int IsCollidable() { return 1; }
	void SetState(int state);

	BOOLEAN IsReturning() { return isReturning; }
};
