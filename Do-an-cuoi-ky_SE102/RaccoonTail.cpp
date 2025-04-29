#include "RaccoonTail.h"
#include "Collision.h"
#include "Goomba.h" 
#include "Koopa.h"  
#include "debug.h" 
#include "QuestionBrick.h"
#include "PiranhaPlant.h"

void CRaccoonTail::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!IsActive()) return;
    if (state == RACCOON_TAIL_STATE_ACTIVE)
    {
        // Move horizontally around the x-axis
        x += vx * dt;
        // Reverse direction if it exceeds the range
        if (x >= startX + RACCOON_TAIL_RANGE)
        {
            vx = -RACCOON_TAIL_SPEED;
            nx = 1;
        }
		else if (x <= startX - RACCOON_TAIL_RANGE)
		{
			SetActive(false); // Set inactive state if out of range
            vx = 0;
            nx = -1;
		}
    }


	//DebugOut(L"[INFO] Raccoon Tail state: %d\n", state);

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CRaccoonTail::OnCollisionWith(LPCOLLISIONEVENT e)
{
    //if (!IsActive()) return;

    CParticle* particle = new CParticle(x, y, PARTICLE_TYPE_HIT);
    CPlayScene* currentScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
    

    if (dynamic_cast<CGoomba*>(e->obj))
    {
        OnCollisionWithGoomba(e);
        currentScene->Add(particle);
    }
    else if (dynamic_cast<CQuestionBrick*>(e->obj))
    {
        OnCollisionWithQuestionBrick(e);
    }
    else if (dynamic_cast<CKoopa*>(e->obj))
    {
        OnCollisionWithKoopa(e);
        currentScene->Add(particle);
    }
    else if (dynamic_cast<CPiranhaPlant*>(e->obj))
    {
        OnCollisionWithPiranhaPlant(e);
        currentScene->Add(particle);
    }
}

void CRaccoonTail::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
    DebugOut(L"[INFO] RaccoonTail hit Goomba\n");
    CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
    goomba->SetState(GOOMBA_STATE_DIE_REVERSE);
}

void CRaccoonTail::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e)
{
    DebugOut(L"[INFO] RaccoonTail hit questionbrick\n");
    CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);
    if (!questionBrick->IsHit())
        questionBrick->Activate();
}

void CRaccoonTail::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
    DebugOut(L"[INFO] RaccoonTail hit Koopa\n");
    CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
    koopa->SetSpeed(nx * KOOPA_SHELL_SPEED / 2, 0); // Set speed for Koopa
    koopa->SetState(KOOPA_STATE_SHELL_REVERSE_JUMP);
}

void CRaccoonTail::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
    DebugOut(L"[INFO] RaccoonTail hit PiranhaPlant\n");
    CPiranhaPlant* piranha = dynamic_cast<CPiranhaPlant*>(e->obj);
    piranha->SetState(PIRANHA_STATE_DIE);
}


void CRaccoonTail::Render()
{
    // Render bounding box for debugging purposes
    RenderBoundingBox();
}

void CRaccoonTail::SetState(int state)
{
    CGameObject::SetState(state);

    switch (state)
    {
    case RACCOON_TAIL_STATE_ACTIVE:
		SetActive(true); // Set active state
		startX = x; // Store the initial position
        vx = RACCOON_TAIL_SPEED; // Set horizontal speed
        break;

    case RACCOON_TAIL_STATE_INACTIVE:
        isActive = false;
        vx = 0; // Stop movement
        break;
    }
}

void CRaccoonTail::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x;
    t = y;
    r = l + 1.f;
    b = t + 16.f;
}
