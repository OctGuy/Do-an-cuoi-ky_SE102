#pragma once
#include "Brick.h"
#include "GameObject.h"
#include "Mario.h"
#include "debug.h"

// Forward declarations instead of includes
class CCoin;
class CPowerUp;
class CKoopa;
class CRaccoonTail;
class CPSwitch;

#define BRICK_BOUNCE_SPEED 0.1f
#define BRICK_BOUNCE_HEIGHT 10.0f
#define BRICK_BOUNCE_TIME 200
#define BRICK_STATE_BOUNCE 100

#define ITEM_TYPE_COIN 1
#define ITEM_TYPE_POWERUP 2
#define ITEM_TYPE_PSWITCH 3

class CQuestionBrick : public CBrick
{
    LPGAMEOBJECT item; // Pointer to Item for easier manipulation
    BOOLEAN hasItem;

    int itemType; // Type of item in the brick
    BOOLEAN isHit;
    float originalY;
    DWORD bounceStart;
    BOOLEAN isBouncingFinished;

public:
    CQuestionBrick(float x, float y, int type, int itemType) : CBrick(x, y, type)
    {
        item = NULL;
        hasItem = false;
        this->itemType = itemType;

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

    BOOLEAN IsHit() { return isHit; }

    void SetState(int state);
    void Activate();
    void ActivateItem();

    CMario* GetPlayer(); 
};