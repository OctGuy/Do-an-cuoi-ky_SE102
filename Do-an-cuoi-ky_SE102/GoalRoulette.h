#pragma once
#include "GameObject.h"

#define CARD_TYPE_MUSHROOM  0
#define CARD_TYPE_PLANT 1
#define CARD_TYPE_STAR  2

#define ID_SPRITE_GOAL_MUSHROOM 50000
#define ID_SPRITE_GOAL_PLANT 50004
#define ID_SPRITE_GOAL_STAR 50005

#define CARD_CHANGE_INTERVAL 100 

class CGoalRoulette : public CGameObject
{
private: 
    int currentCard;
    ULONGLONG cardChangeTimer;
public:
    CGoalRoulette(float x, float y);

    void Update(DWORD dt, std::vector<LPGAMEOBJECT>* coObjects = nullptr) override;
    void Render() override;
    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;

	int IsCollidable() override { return 1; }
	int IsBlocking() override { return 0; }
};
