#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_BRICK_NORMAL 10006
#define ID_ANI_BRICK_FLOOR_BOTTOM_LEFT 10000
#define ID_ANI_BRICK_FLOOR_BOTTOM_MIDDLE 10001
#define ID_ANI_BRICK_FLOOR_BOTTOM_RIGHT 10002
#define ID_ANI_BRICK_FLOOR_TOP_LEFT 10003
#define ID_ANI_BRICK_FLOOR_TOP_MIDDLE 10004
#define ID_ANI_BRICK_FLOOR_TOP_RIGHT 10005

#define ID_ANI_BRICK_CONVEX 10010

#define BRICK_WIDTH 16
#define BRICK_BBOX_WIDTH 16
#define BRICK_BBOX_HEIGHT 16

#define BRICK_TYPE_NORMAL 6
#define BRICK_TYPE_FLOOR_BOTTOM_LEFT 0
#define BRICK_TYPE_FLOOR_BOTTOM_MIDDLE 1
#define BRICK_TYPE_FLOOR_BOTTOM_RIGHT 2
#define BRICK_TYPE_FLOOR_TOP_LEFT 3
#define BRICK_TYPE_FLOOR_TOP_MIDDLE 4
#define BRICK_TYPE_FLOOR_TOP_RIGHT 5
#define BRICK_TYPE_CONVEX 10

class CBrick : public CGameObject {
protected:
	int type;
public:
	CBrick(float x, float y, int type) : CGameObject(x, y), type(type) {}
	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	int GetAnimationid();
};