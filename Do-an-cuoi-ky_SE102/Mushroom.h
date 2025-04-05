#pragma once
#include "GameObject.h"

#define ID_TEX_MUSHROOM 100
#define ID_ANI_MUSHROOM 12000
#define MUSHROOM_BBOX_WIDTH 16
#define MUSHROOM_BBOX_HEIGHT 16

#define MUSHROOM_STATE_WALKING 100
#define ID_SPRITE_MUSHROOM 50000
class CMushroom : public CGameObject
{
protected:
    float ax;
    float ay;
    bool isOnPlatform;

    virtual void OnNoCollision(DWORD dt);
    virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
    CMushroom(float x, float y);
    void Render() override;
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;

    void SetState(int state) override;
    int IsCollidable() { return 1; };
    int IsBlocking() { return 0; }
};
