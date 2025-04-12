#pragma once
#include "Brick.h"
#include "GameObject.h"
#include "Mario.h"
#include "debug.h"

class CQuestionBrick : public CBrick
{
	LPGAMEOBJECT item; //Pointer to Item for easier manipulation
	BOOLEAN hasItem;
	BOOLEAN isHit;
public:
	CQuestionBrick(float x, float y, int type) : CBrick(x, y, type) 
	{
		item = NULL;
		hasItem = false;
		isHit = false;
	}
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	//void GetBoundingBox(float& l, float& t, float& r, float& b);
	//int IsBlocking();

	void OnNoCollision(DWORD dt) {
		return;
	}

	void OnCollisionWith(LPCOLLISIONEVENT e);

	void SetItem(LPGAMEOBJECT item) {
		this->item = item;
		if (item != NULL)
			hasItem = true;
	}
};