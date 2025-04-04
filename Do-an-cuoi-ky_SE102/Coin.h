#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#include "debug.h"
#include "Mario.h"
#include "Game.h"
#include "PlayScene.h"

#define ID_ANI_COIN 11000

#define	COIN_WIDTH 10
#define COIN_BBOX_WIDTH 10
#define COIN_BBOX_HEIGHT 14

#define COIN_BOUNCE_SPEED 0.2f
#define COIN_BOUNCE_HEIGHT 50.0f
#define COIN_BOUNCE_TIME 2000

#define COIN_STATE_BOUNCE 100

class CCoin : public CGameObject {
	float originalY;
	DWORD bounceStart;
public:
	CCoin(float x, float y) : CGameObject(x, y) 
	{
		originalY = y;
		bounceStart = 0;
	}
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);

	void SetState(int state);
	int IsBlocking() { return 0; }

	void CollectCoin();
};