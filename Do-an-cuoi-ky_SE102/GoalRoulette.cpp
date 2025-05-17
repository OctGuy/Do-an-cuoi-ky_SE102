#include "GoalRoulette.h"
#include "AssetIDs.h"

CGoalRoulette::CGoalRoulette(float x, float y) : CGameObject(x, y)
{
    currentCard = CARD_TYPE_MUSHROOM;
    cardChangeTimer = GetTickCount64();
}

void CGoalRoulette::Update(DWORD dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	ULONGLONG now = GetTickCount64();
	if (now - cardChangeTimer > CARD_CHANGE_INTERVAL)
	{
		cardChangeTimer = now;
		currentCard = (currentCard + 1) % 3; // Cycle through the card types
	}
}

void CGoalRoulette::Render()
{
    CSprites* s = CSprites::GetInstance();
	int aniID = -1;

	switch (currentCard)
	{
	default:
	case CARD_TYPE_MUSHROOM:
		aniID = ID_SPRITE_GOAL_MUSHROOM;
		break;
	case CARD_TYPE_PLANT:
		aniID = ID_SPRITE_GOAL_PLANT;
		break;
	case CARD_TYPE_STAR:
		aniID = ID_SPRITE_GOAL_STAR;
		break;
	}

	if (aniID != -1)
	{
		s->Get(aniID)->Draw(x, y);
	}
	else
	{
		DebugOut(L"[ERROR] CGoalRoulette::Render: Invalid animation ID\n");
	}
    RenderBoundingBox();
}

void CGoalRoulette::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - 8;
    top = y - 8;
    right = x + 8;
    bottom = y + 8;
}
