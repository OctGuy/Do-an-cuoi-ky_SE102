#pragma once
#include "Platform.h"

#define MOVING_PLATFORM_HORIZONTAL_SPEED 0.05f
#define MOVING_PLATFORM_VERTICAL_SPEED 0.05f

using namespace std;

class CMovingPlatform : public CPlatform
{
private:
	void OnCollisionWithMario(LPCOLLISIONEVENT e);
public:
	CMovingPlatform(float x, float y,
		float cell_width, float cell_height, int length,
		int sprite_id_begin, int sprite_id_middle, int sprite_id_end) : 
		CPlatform(x, y, cell_width, cell_height, length, sprite_id_begin, sprite_id_middle, sprite_id_end) 
	{ 
		this->vx = -MOVING_PLATFORM_HORIZONTAL_SPEED;
		this->vy = 0;
	}

	virtual void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	void GetBoundingBox(float& l, float& t, float& r, float& b) override;

	void OnNoCollision(DWORD dt);

	void OnCollisionWith(LPCOLLISIONEVENT e);

	int IsCollidable() { return 1; }
	int IsBlocking() { return 1; }
	int IsTangible() { return 0; }
	int IsDirectionColliable(float nx, float ny) override { return 1; }
};