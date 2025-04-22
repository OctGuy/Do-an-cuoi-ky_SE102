#pragma once
#include "GameObject.h"

#define RACCOON_TAIL_STATE_INACTIVE 0
#define RACCOON_TAIL_STATE_ACTIVE 1

#define RACCOON_TAIL_SPEED 0.1f // Speed of horizontal movement
#define RACCOON_TAIL_RANGE 20.0f // Range of horizontal movement

class CRaccoonTail : public CGameObject
{
protected:
    float startX; // Initial x position for horizontal movement

public:
    CRaccoonTail(float x, float y) : CGameObject(x, y)
    {
        this->startX = x;
        this->state = RACCOON_TAIL_STATE_INACTIVE;
        this->isActive = true;
    }

    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) override;
    void Render() override;
    void SetState(int state) override;
    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;

    int IsCollidable() { return 1; }
    int IsBlocking() { return 0; }
};
