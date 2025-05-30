#pragma once

#include "GameObject.h"

// 
// The most popular type of object in Mario! 
// 
class CPlatform : public CGameObject
{
protected:
	int length;				// Unit: cell 
	float cellWidth;
	float cellHeight;
	int spriteIdBegin, spriteIdMiddle, spriteIdEnd;

public:
	CPlatform(float x, float y,
		float cell_width, float cell_height, int length,
		int sprite_id_begin, int sprite_id_middle, int sprite_id_end) : CGameObject(x, y)
	{
		this->length = length;
		this->cellWidth = cell_width;
		this->cellHeight = cell_height;
		this->spriteIdBegin = sprite_id_begin;
		this->spriteIdMiddle = sprite_id_middle;
		this->spriteIdEnd = sprite_id_end;
	}

	virtual void Render();
	virtual void Update() {}
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void RenderBoundingBox();
	virtual void OnCollisionWith(LPCOLLISIONEVENT e) {}
	virtual void OnNoCollision(DWORD dt) {}

	virtual int IsDirectionColliable(float nx, float ny);
	virtual int IsCollidable() override { return 1; }
};

typedef CPlatform* LPPLATFORM;