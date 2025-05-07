#include "QuestionBrick.h"
#include "Coin.h"
#include "PowerUp.h"
#include "Koopa.h"
#include "RaccoonTail.h"
#include "PSwitch.h"
#include "Game.h"
#include "PlayScene.h"
#include "Animations.h"
#include "Collision.h"

CMario* CQuestionBrick::GetPlayer()
{
	CGame* game = CGame::GetInstance();
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(playScene->GetPlayer());
	return mario;
}

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
    if (e->nx != 0 && dynamic_cast<CKoopa*>(e->src_obj))
    {
		Activate();
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
                if (itemType != ITEM_TYPE_COIN)
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


void CQuestionBrick::Activate()
{
    if (!isHit)
    {
        CMario* mario = GetPlayer();
        CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
        SetState(BRICK_STATE_BOUNCE);

        if (itemType == ITEM_TYPE_COIN)
        {
            item = new CCoin(x, y - 16.f);
            playScene->Add(item);
			ActivateItem();
		}
        else if (itemType == ITEM_TYPE_PSWITCH)
        {
            item = new CPSwitch(x, y - 13.f);
            playScene->Add(item);
            CGame* game = CGame::GetInstance();
            CPlayScene* scene = (CPlayScene*)game->GetCurrentScene();
			CParticle* smoke = new CParticle(x, y - 13.f, PARTICLE_TYPE_SMOKE);
			playScene->Add(smoke);
        }
        else
        {
			item = new CPowerUp(x, y - 3.f);
            if (mario->GetLevel() == MARIO_LEVEL_SMALL)
                dynamic_cast<CPowerUp*>(item)->SetType(POWER_UP_TYPE_MUSHROOM);
            else
                dynamic_cast<CPowerUp*>(item)->SetType(POWER_UP_TYPE_LEAF);
            playScene->Add(item);
        }
    }
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
    item->SetActive(true);
    item->SetState(100); //100 is STATE_ACTIVE for all item (lazy implementation)
	item = NULL; // Set item to NULL to prevent access error (PAINFUL LESSON)
}



