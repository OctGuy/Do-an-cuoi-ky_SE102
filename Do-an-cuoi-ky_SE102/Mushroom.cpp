#include "Mushroom.h"
#include "Textures.h"


CMushroom::CMushroom(float x, float y) : CGameObject(x, y)
{
    this->ax = 0;
    this->ay = 0;
	isActive = false;
}

void CMushroom::Render()
{
    // Render the star sprite
    //CAnimations* animations = CAnimations::GetInstance();
    //animations->Get(ID_ANI_MUSHROOM)->Render(x, y);
    //RenderBoundingBox();

	CSprites* sprites = CSprites::GetInstance();
	sprites->Get(ID_SPRITE_MUSHROOM)->Draw(x, y + 2); //added 2 because the mushroo was floating for some reason
	
    //RenderBoundingBox();
}

void CMushroom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    // Apply gravity
    vy += ay * dt;
    vx += ax * dt;

    if (y <= originalY - MUSHROOM_BBOX_HEIGHT)
    {
        //DebugOut(L"Maximum height reached");
        y = originalY - MUSHROOM_BBOX_HEIGHT;
        SetState(MUSHROOM_STATE_WALKING);
    }
    CGameObject::Update(dt, coObjects);

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMushroom::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - MUSHROOM_BBOX_WIDTH / 2;
    top = y - MUSHROOM_BBOX_HEIGHT / 2;
    right = left + MUSHROOM_BBOX_WIDTH;
    bottom = top + MUSHROOM_BBOX_HEIGHT;
}

void CMushroom::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
    //isOnPlatform = false;
}

void CMushroom::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (!e->obj->IsBlocking()) return;
    if (dynamic_cast<CMushroom*>(e->obj)) return;

    if (e->ny != 0)
    {
        vy = 0;
    }
    else if (e->nx != 0)
    {
        vx = -vx;  // Reverse direction when hitting a wall
    }
}

void CMushroom::SetState(int state)
{
    CGameObject::SetState(state);

    switch (state)
    {
        case MUSHROOM_STATE_WALKING:
            vx = -MUSHROOM_WALKING_SPEED;  // Start moving left like Goomba
		    ay = MUSHROOM_GRAVITY;  // Apply gravity
            break;

        case MUSHROOM_STATE_RISE:
            originalY = y;
            vy = -MUSHROOM_RISE_SPEED;  // Start moving up
            break;
    }
}
