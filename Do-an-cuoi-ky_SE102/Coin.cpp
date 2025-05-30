#include "Coin.h"
#include "debug.h"

void CCoin::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_COIN)->Render(x, y);
	//RenderBoundingBox();
}

void CCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == COIN_STATE_BOUNCE) {
		if (GetTickCount64() - bounceStart < COIN_BOUNCE_TIME)
		{
			y += vy * dt;
			if (y <= originalY - COIN_BOUNCE_HEIGHT)
			{
				y = originalY - COIN_BOUNCE_HEIGHT;
				vy = COIN_BOUNCE_SPEED;
			}
			else if (y > originalY && vy > 0)
			{
				CollectCoin();
			}
		}
		else
		{
			y = originalY;
			vy = 0;
		}
	}
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - COIN_BBOX_WIDTH / 2;
	t = y - COIN_BBOX_HEIGHT / 2;
	r = l + COIN_BBOX_WIDTH;
	b = t + COIN_BBOX_HEIGHT;
}

void CCoin::SetState(int state)
{
	switch (state)
	{
		case COIN_STATE_BOUNCE:
			originalY = y - COIN_BBOX_HEIGHT; //This happen after question brick update new y position for coin
			bounceStart = GetTickCount64();
			vy = -COIN_BOUNCE_SPEED;
			break;
	}

	CGameObject::SetState(state);
}

void CCoin::CollectCoin()
{
	// Logic to handle coin collection
	//I wanted mario to call oncollisionwithcoin function instead but this will do(Hopefully)
	CGame* game = CGame::GetInstance();
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(playScene->GetPlayer());
	CParticle* particle = new CParticle(x, y, PARTICLE_TYPE_POINT, 100);
	playScene->Add(particle);
	if (playScene)
	{
		if (mario)
		{
			mario->AddCoin();
			mario->AddPoint(100);
		}
	}

	this->Delete();
}