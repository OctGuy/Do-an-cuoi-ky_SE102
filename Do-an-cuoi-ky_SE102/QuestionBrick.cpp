#include "QuestionBrick.h"
#include "Coin.h"
#include "PowerUp.h"
#include "Koopa.h"
#include "ParaTroopa.h"
#include "RaccoonTail.h"
#include "PSwitch.h"
#include "Game.h"
#include "PlayScene.h"
#include "Animations.h"
#include "Collision.h"
#include "1UpMushroom.h"

CMario* CQuestionBrick::GetPlayer()
{
	CGame* game = CGame::GetInstance();
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(playScene->GetPlayer());
	return mario;
}

void CQuestionBrick::Render()
{
    CAnimations* animations = CAnimations::GetInstance();
    int id = -1;
    if (bounceStart) {
        id = ID_ANI_QUESTION_BRICK_BOUNCE;
        animations->Get(id)->Render(x, y);
    }
    else if (isHit) {
        id = ID_ANI_QUESTION_BRICK_INACTIVE;
        animations->Get(id)->Render(x, y);
    }
    else
	    CBrick::Render();
}

void CQuestionBrick::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (e->nx != 0 && (dynamic_cast<CKoopa*>(e->src_obj) 
                        || dynamic_cast<CParaTroopa*>(e->src_obj)))
    {
	    Activate();
    }

    if (e->ny > 0 && (dynamic_cast<CKoopa*>(e->obj)
        || dynamic_cast<CParaTroopa*>(e->obj))) {
        if (dynamic_cast<CKoopa*>(e->obj))
			Koopa = dynamic_cast<CKoopa*>(e->obj);
		else if (dynamic_cast<CParaTroopa*>(e->obj))
			Koopa = dynamic_cast<CParaTroopa*>(e->obj);
    }
}

void CQuestionBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (isHit)
    {
        if (GetTickCount64() - bounceStart >= BRICK_BOUNCE_TIME)
        {
            if (!isBouncingFinished) // After brick stop bouncing, activate non-coin item
            {
                isBouncingFinished = true;
                if (itemType != ITEM_TYPE_COIN)
                {
                    ActivateItem();
                }
                //Reset aniamtion
                CAnimations* animations = CAnimations::GetInstance();
				animations->Get(ID_ANI_QUESTION_BRICK_BOUNCE)->Reset();
            }
            bounceStart = 0;
        }
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CQuestionBrick::Activate()
{
    if (!isHit)
    {
        if (dynamic_cast<CKoopa*>(Koopa)) {
			dynamic_cast<CKoopa*>(Koopa)->SetState(KOOPA_STATE_SHELL_REVERSE_JUMP);
        }
        else if (dynamic_cast<CParaTroopa*>(Koopa)) {
            dynamic_cast<CParaTroopa*>(Koopa)->SetState(PARATROOPA_STATE_SHELL_REVERSE_JUMP);
        }

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
        else if (itemType == ITEM_TYPE_1UP_MUSHROOM)
        {
            item = new C1UpMushroom(x, y - 8.f);
            dynamic_cast<CPowerUp*>(item)->SetType(POWER_UP_TYPE_1UP_MUSHROOM);
			playScene->Add(item);
        }
        else
        {
			item = new CPowerUp(x, y - 8.f);
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
            }
            break;
    }
}

void CQuestionBrick::ActivateItem()
{
    if (item == nullptr) return;
	//THE ORDER OF THESE FUNCTIONS MATTER
    item->SetState(100); //100 is STATE_ACTIVE for all item (lazy implementation)
    item->SetActive(true);
    item = nullptr; // Set item to NULL to prevent access error (PAINFUL LESSON)
}



