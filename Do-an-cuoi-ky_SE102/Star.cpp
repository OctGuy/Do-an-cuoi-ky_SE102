#include "Star.h"
#include "Textures.h"

#define STAR_GRAVITY 0.002f
#define STAR_WALKING_SPEED 0.1f
#define STAR_JUMP_SPEED 0.25f
#define STAR_JUMP_INTERVAL 1000  // Jump every 2 seconds
#define STAR_JUMP_DEFLECT_SPEED 0.4f

CStar::CStar(float x, float y) : CGameObject(x, y)
{
    this->ax = 0;
    this->ay = STAR_GRAVITY;
    nextJumpTime = GetTickCount64() + STAR_JUMP_INTERVAL;
    SetState(STAR_STATE_WALKING);
}

void CStar::Render()
{
    // Render the star sprite
    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(ID_ANI_STAR)->Render(x, y);
    //RenderBoundingBox();
}

void CStar::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    // Apply gravity
    vy += ay * dt;
    vx += ax * dt;

    // Check if it's time to jump
    if (isOnPlatform && GetTickCount64() > nextJumpTime)
    {
        vy = -STAR_JUMP_SPEED;
        isOnPlatform = false;
        nextJumpTime = GetTickCount64() + STAR_JUMP_INTERVAL;
    }

    //CGameObject::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CStar::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - STAR_BBOX_WIDTH / 2;
    top = y - STAR_BBOX_HEIGHT / 2;
    right = left + STAR_BBOX_WIDTH;
    bottom = top + STAR_BBOX_HEIGHT;
}

void CStar::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
    isOnPlatform = false;
}

void CStar::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (!e->obj->IsBlocking()) return;
    if (dynamic_cast<CStar*>(e->obj)) return;

    if (e->ny != 0)
    {
        vy = 0;
        if (e->ny < 0)
        {
            isOnPlatform = true;
        }
    }
    else if (e->nx != 0)
    {
        vx = -vx;  // Reverse direction when hitting a wall
    }
}

void CStar::SetState(int state)
{
    CGameObject::SetState(state);

    switch (state)
    {
    case STAR_STATE_WALKING:
        vx = -STAR_WALKING_SPEED;  // Start moving left like Goomba
        break;
    }
}
