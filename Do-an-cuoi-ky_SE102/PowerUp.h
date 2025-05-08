#pragma once
#include "GameObject.h"
#include "PlayScene.h"
#include "Mario.h"
#include "Game.h"

#define POWER_UP_TYPE_MUSHROOM 1
#define POWER_UP_TYPE_LEAF 2

#define LEAF_STATE_RISE 100
#define LEAF_STATE_FALLING 101
#define ID_SPRITE_LEAF_RIGHT 50001
#define ID_SPRITE_LEAF_LEFT 50002

#define LEAF_RISE_HEIGHT 25
#define LEAF_SWAY_DISTANCE 40

#define LEAF_GRAVITY 0.01f
#define LEAF_SWAYING_SPEED 0.04f
#define LEAF_RISE_SPEED 0.08f


///////////////
#define MUSHROOM_BBOX_WIDTH 16
#define MUSHROOM_BBOX_HEIGHT 16

#define MUSHROOM_STATE_RISE 100
#define MUSHROOM_STATE_WALKING 101
#define ID_SPRITE_MUSHROOM 50000

#define MUSHROOM_GRAVITY 0.002f
#define MUSHROOM_WALKING_SPEED 0.05f
#define MUSHROOM_RISE_SPEED 0.02f


class CPowerUp : public CGameObject
{
protected:
    float ax;
    float ay;
    float originalY;
	float originalX;
    //bool isOnPlatform;

	int type; // 1: mushroom, 2: leaf

    virtual void OnNoCollision(DWORD dt);
    virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
    CPowerUp(float x, float y);

    void Render() override;
	void RenderMushroom();
	void RenderLeaf();

    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	void UpdateMushroom();
	void UpdateLeaf();
    virtual void Reload();

    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
	int GetType() { return type; }
	void SetType(int type) { this->type = type; }

    void SetState(int state) override;
    void SetStateMushroom(int state);
    void SetStateLeaf(int state);

    int IsCollidable() { return type == POWER_UP_TYPE_LEAF ? 0 : 1; }
    int IsBlocking() { return 0; }
};
