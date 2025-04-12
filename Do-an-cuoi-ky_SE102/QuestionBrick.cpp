#include "QuestionBrick.h"

void CQuestionBrick::Render()
{
	CBrick::Render();
}

void CQuestionBrick::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (e->ny > 0)
    {
        if (e->obj->GetState() != MARIO_STATE_DIE)
        {
            if (item != NULL && !isHit)
            {
                item->SetPosition(x, y - BRICK_BBOX_HEIGHT);
                isHit = true;
                //item->SetState(ITEM_STATE_BOUNCE);
                //item->SetSpeed(0, -0.1f);
            }
            //SetState(BRICK_STATE_BOUNCE);
        }
    }
}


void CQuestionBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	CCollision::GetInstance()->Process(this, dt, coObjects);
	//if (item != NULL)
	//	item->Update(dt);
}
