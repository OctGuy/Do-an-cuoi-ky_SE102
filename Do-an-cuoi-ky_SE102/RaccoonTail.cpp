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
        if (x > startX + RACCOON_TAIL_RANGE)
        {
            vx = -vx;
        }
		else if (x < startX - RACCOON_TAIL_RANGE)
		{
			SetActive(false); // Set inactive state if out of range
		}
    }


	//DebugOut(L"[INFO] Raccoon Tail state: %d\n", state);

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CRaccoonTail::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!IsActive()) return;
	else if (dynamic_cast<CGoomba*>(e->obj)) 
	{
		DebugOut(L"[INFO] RaccoonTail hit Goomba\n");
		CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
		goomba->SetState(GOOMBA_STATE_DIE_REVERSE); 
	}
	else if (dynamic_cast<CQuestionBrick*>(e->obj)) 
	{
		DebugOut(L"[INFO] RaccoonTail hit questionbrick\n");
        CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);
        if (!questionBrick->IsHit())
            questionBrick->Activate();
	}
	else if (dynamic_cast<CKoopa*>(e->obj))
	{
		DebugOut(L"[INFO] RaccoonTail hit Koopa\n");
		CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
		koopa->SetState(KOOPA_STATE_SHELL_REVERSE_IDLE);
	}
	else if (dynamic_cast<CPiranhaPlant*>(e->obj))
	{
		DebugOut(L"[INFO] RaccoonTail hit PiranhaPlant\n");
		CPiranhaPlant* piranha = dynamic_cast<CPiranhaPlant*>(e->obj);
		piranha->SetState(PIRANHA_STATE_DIE);
	}
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
        isActive = true;
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
    r = l + 8; // Example width
    b = t + 8; // Example height
}
