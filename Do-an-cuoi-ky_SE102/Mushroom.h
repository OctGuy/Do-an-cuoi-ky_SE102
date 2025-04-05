#pragma once
#include "GameObject.h"

#define ID_TEX_STAR 100
#define ID_ANI_STAR 12000
#define STAR_BBOX_WIDTH 16
#define STAR_BBOX_HEIGHT 16
#define INVINCIBILITY_DURATION 5000 // 5 seconds

#define STAR_STATE_WALKING 100

class CStar : public CGameObject
{
protected:
    float ax;
    float ay;
    bool isOnPlatform;
    ULONGLONG nextJumpTime;

    virtual void OnNoCollision(DWORD dt);
    virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
    CStar(float x, float y);
    void Render() override;
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;

    void SetState(int state) override;
    int IsCollidable() { return 1; };
    int IsBlocking() { return 0; }
};
