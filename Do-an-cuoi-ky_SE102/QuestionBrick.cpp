#include "QuestionBrick.h"

void CQuestionBrick::Render()
{
    if (isHit) {
        CAnimations* animations = CAnimations::GetInstance();
        int id = ID_ANI_QUESTION_BRICK_INACTIVE;
        animations->Get(id)->Render(x, y);
    }
    else
	    CBrick::Render();
}

void CQuestionBrick::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (e->ny > 0)
    {
        if (e->obj->GetState() != MARIO_STATE_DIE)
        {
            if (!isHit)
            {
				SetState(BRICK_STATE_BOUNCE);
				if (dynamic_cast<CCoin*>(item)) //Only activate coin immidiately
				    ActivateItem();
            }
        }
    }
}


void CQuestionBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (isHit)
    {
        if (GetTickCount64() - bounceStart < BRICK_BOUNCE_TIME)
        {
            y += vy * dt;
            if (y <= originalY - BRICK_BOUNCE_HEIGHT)
            {
                y = originalY - BRICK_BOUNCE_HEIGHT;
                vy = BRICK_BOUNCE_SPEED;
            }
            else if (y >= originalY)
            {
                y = originalY;
                vy = 0;
            }
        }
        else
        {
            if (!isBouncingFinished) // After brick stop bouncing, activate non-coin item
            {
                isBouncingFinished = true;
                if (!dynamic_cast<CCoin*>(item)) 
                {
                    ActivateItem();
                }
            }
            y = originalY;
            vy = 0;
        }
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CQuestionBrick::SetState(int state)
{
    CGameObject::SetState(state);
    switch (state)
    {
        case BRICK_STATE_BOUNCE:
            if (!isHit)
            {
                isHit = true;
                bounceStart = GetTickCount64();
                vy = -BRICK_BOUNCE_SPEED;
            }
            break;
    }
}

void CQuestionBrick::ActivateItem()
{
    if (item == NULL) return;
	//THE ORDER OF THESE FUNCTIONS MATTER
    item->SetPosition(x, y);
    item->SetActive(true);
    item->SetState(100); //100 is STATE_ACTIVE for all item (lazy implementation)
	item = NULL; // Set item to NULL to prevent access error (PAINFUL LESSON)
}



