#pragma once
#include "GameObject.h"

#define RACCOON_TAIL_STATE_INACTIVE 200
#define RACCOON_TAIL_STATE_ACTIVE 100

#define RACCOON_TAIL_SPEED 0.20f 
#define RACCOON_TAIL_RANGE 20.0f

class CRaccoonTail : public CGameObject
{
protected:
    float startX; // Initial x position for horizontal movement
    bool isActive; // Flag to indicate if the tail is currently active

public:
    CRaccoonTail(float x, float y) : CGameObject(x, y)
    {
        this->startX = x;
        this->isActive = false; // Initially inactive
        this->SetState(RACCOON_TAIL_STATE_INACTIVE);
        nx = -1;
    }


    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) override;
    void Render() override;
    void SetState(int state) override;
    void GetBoundingBox(float& l, float& t, float& r, float& b);

    void OnCollisionWith(LPCOLLISIONEVENT e) override;
    void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
    void OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e);
    void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
    void OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e);

    int IsCollidable() { return 1; }
    int IsBlocking() { return 0; }
    //ool IsActive() { return isActive; }
};
