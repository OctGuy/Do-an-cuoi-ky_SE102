#include "PowerUp.h"
#include "Textures.h"
#include "Animations.h"

#define ID_ANI_RED_MUSHROOM_RISE 14000
#define MUSHROOM_RISE_ANIMATION_TIME 700

CPowerUp::CPowerUp(float x, float y) : CGameObject(x, y)
{
    this->ax = 0;
    this->ay = 0;
    type = 2; // Default type
    isActive = false;
    isRising = false;
    rise_start = 0;
}

void CPowerUp::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - MUSHROOM_BBOX_WIDTH / 2;
    top = y - MUSHROOM_BBOX_HEIGHT / 2;
    right = left + MUSHROOM_BBOX_WIDTH;
    bottom = top + MUSHROOM_BBOX_HEIGHT;
}

void CPowerUp::Render()
{
    if (type == POWER_UP_TYPE_MUSHROOM)
    {
        RenderMushroom();
    }
    else if (type == POWER_UP_TYPE_LEAF)
    {
        RenderLeaf();
    }
    //RenderBoundingBox();
}

void CPowerUp::RenderMushroom()
{
    CAnimations* animations = CAnimations::GetInstance();
    CSprites* sprites = CSprites::GetInstance();

    if (isRising)
    {
        // Play the rising animation
        animations->Get(ID_ANI_RED_MUSHROOM_RISE)->Render(x, y - 3.f);
    }
    else
    {
        // Draw the regular sprite
        sprites->Get(ID_SPRITE_MUSHROOM)->Draw(x, y); //added 2 because the mushroom was floating for some reason
    }
}

void CPowerUp::RenderLeaf()
{
    CSprites* sprites = CSprites::GetInstance();
    if (vx >= 0)
        sprites->Get(ID_SPRITE_LEAF_RIGHT)->Draw(x, y);
    else
        sprites->Get(ID_SPRITE_LEAF_LEFT)->Draw(x, y);
}

void CPowerUp::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    // Apply gravity
    vy += ay * dt;
    vx += ax * dt;

    if (type == POWER_UP_TYPE_MUSHROOM || type == POWER_UP_TYPE_1UP_MUSHROOM)
    {
        UpdateMushroom();
    }
    else if (type == POWER_UP_TYPE_LEAF)
    {
        UpdateLeaf();
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CPowerUp::UpdateMushroom()
{
    // If mushroom is in rising animation state
    if (isRising)
    {
        // Check if the animation time has passed
        ULONGLONG now = GetTickCount64();
        if (now - rise_start >= MUSHROOM_RISE_ANIMATION_TIME)
        {
            // Animation completed, switch to walking state
            isRising = false;
            SetState(MUSHROOM_STATE_WALKING);
        }
    }
}

void CPowerUp::UpdateLeaf()
{
    // Update leaf floating animation
    if (y <= originalY - LEAF_RISE_HEIGHT)
    {
        //DebugOut(L"Maximum height reached");
        y = originalY - LEAF_RISE_HEIGHT;
        SetState(LEAF_STATE_FALLING);
    }
    else if (x - originalX >= LEAF_SWAY_DISTANCE && vx > 0) // Moving right and reached sway limit
    {
        vx = -LEAF_SWAYING_SPEED; // Reverse direction to left
    }
    else if (x <= originalX && vx < 0) // Moving left and reached sway limit
    {
        vx = LEAF_SWAYING_SPEED; // Reverse direction to right
    }
}

void CPowerUp::Reload()
{
    this->Delete();
}

void CPowerUp::OnNoCollision(DWORD dt)
{
    if (!isRising) // Only move if not in rising animation
    {
        x += vx * dt;
        y += vy * dt;
    }
}

void CPowerUp::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (!e->obj->IsBlocking()) return;
    if (dynamic_cast<CPowerUp*>(e->obj)) return;

    if (e->ny != 0)
    {
        vy = 0;
    }
    else if (e->nx != 0)
    {
        if (e->nx > 0)
            vx = MUSHROOM_WALKING_SPEED;
        else
            vx = -MUSHROOM_WALKING_SPEED;
    }
}

void CPowerUp::SetState(int state)
{
    CGameObject::SetState(state);

    if (type == POWER_UP_TYPE_MUSHROOM || type == POWER_UP_TYPE_1UP_MUSHROOM)
    {
        SetStateMushroom(state);
    }
    else if (type == POWER_UP_TYPE_LEAF)
    {
        SetStateLeaf(state);
    }
}

void CPowerUp::SetStateMushroom(int state)
{
    CPlayScene* currentScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
    CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());
    float marioX, marioY;
    mario->GetPosition(marioX, marioY);

	CGameObject::SetState(state);

    switch (state)
    {
    case MUSHROOM_STATE_WALKING:
        if (marioX > x)
            nx = -1;
        else
            nx = 1;
        vx = nx * MUSHROOM_WALKING_SPEED;  // Start moving left like Goomba
        ay = MUSHROOM_GRAVITY;  // Apply gravity
        isRising = false;
        break;
    case MUSHROOM_STATE_RISE:
        y -= MUSHROOM_BBOX_HEIGHT/2;
        isRising = true;
        rise_start = GetTickCount64();
        // Reset velocity since we don't want the mushroom to move
        vx = 0;
        vy = 0;
        ay = 0;
        break;
    }
}

void CPowerUp::SetStateLeaf(int state)
{
    CGameObject::SetState(state);

    switch (state)
    {
    case LEAF_STATE_FALLING:
        vx = LEAF_SWAYING_SPEED;  // Sway right first
        ay = 0;  // No gravity when swaying
        vy = LEAF_GRAVITY;
        break;
    case LEAF_STATE_RISE:
        originalY = y;
        originalX = x;
        vy = -LEAF_RISE_SPEED;  // Start moving up
        break;
    }
}
