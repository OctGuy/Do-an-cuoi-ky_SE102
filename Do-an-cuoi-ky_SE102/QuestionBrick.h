#pragma once
#include "Brick.h"
#include "GameObject.h"
#include "Mario.h" // Ensure this is included to define CMario
#include "debug.h"
#include "Coin.h"
#include "PowerUp.h"
#include "Koopa.h"
#include "RaccoonTail.h"

#define BRICK_BOUNCE_SPEED 0.1f
#define BRICK_BOUNCE_HEIGHT 10.0f
#define BRICK_BOUNCE_TIME 200
#define BRICK_STATE_BOUNCE 100

class CQuestionBrick : public CBrick
{
    LPGAMEOBJECT item; // Pointer to Item for easier manipulation
    BOOLEAN hasItem;
    BOOLEAN isHit;
    float originalY;
    DWORD bounceStart;
    BOOLEAN isBouncingFinished;

public:
    CQuestionBrick(float x, float y, int type) : CBrick(x, y, type)
    {
        item = NULL;
        hasItem = false;
        isHit = false;
        originalY = y;
        bounceStart = 0;
        isBouncingFinished = false;
    }
    void Render();
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);

    void OnNoCollision(DWORD dt) {
        return;
    }

    void OnCollisionWith(LPCOLLISIONEVENT e);
    //void OnCollisionWithTail(LPCOLLISIONEVENT e);

    void SetItem(LPGAMEOBJECT item) {
        this->item = item;
        if (item != NULL)
            hasItem = true;
    }

    BOOLEAN IsHit() { return isHit; }

    void SetState(int state);
    void Activate();
    void ActivateItem();

    CMario* GetPlayer(); // No changes needed here
};