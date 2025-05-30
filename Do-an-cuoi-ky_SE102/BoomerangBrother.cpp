#include "BoomerangBrother.h"
#include "Mario.h"
#include "PlayScene.h"
#include "Boomerang.h"

void CBoomerangBrother::Render()
{
	int aniId = -1;
	if (state == BOOMERANG_BROTHER_STATE_DIE)
	{
		aniId = ID_ANI_BOOMERANG_BROTHER_DIE;
	}
	else if (nx == 1)
	{
		if (isAiming)
			aniId = ID_ANI_BOOMERANG_BROTHER_AIM_RIGHT;
		else 
			aniId = ID_ANI_BOOMERANG_BROTHER_THROW_RIGHT;
	}
	else
	{
		if (isAiming)
			aniId = ID_ANI_BOOMERANG_BROTHER_AIM_LEFT;
		else 
			aniId = ID_ANI_BOOMERANG_BROTHER_THROW_LEFT;
	}

	if (aniId != -1)
	{
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	}
	else
	{
		DebugOut(L"[ERROR] Boomerang Brother animation ID is not set\n");
	}

	//RenderBoundingBox();
}

void CBoomerangBrother::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt; 

	ULONGLONG now = GetTickCount64();

	if (state == BOOMERANG_BROTHER_STATE_DIE)
	{ 
		y += vy * dt; 
		if (now - die_start > BOOMERANG_BROTHER_DIE_TIMEOUT)
		{
			this->Delete();
		}
		return;
	}

	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	CMario* player = dynamic_cast<CMario*>(currentScene->GetPlayer());

	float playerX, playerY;
	if (player)
	{
		player->GetPosition(playerX, playerY);
	}

	float dxPlayer = playerX - this->x;

	//Track mario postiion to turn left or right
	if (dxPlayer < 0)	nx = -1;
	else	nx = 1;

	//Logic for patrolling, if hit left edge, wait, move right, if hit right edge, wait, move left
	if(fabs(x - originalX) >= BOOMERANG_BROTHER_PATROL_DISTANCE)
	{
		if(!stopStartTime)
			stopStartTime = now;

		if (now - stopStartTime >= BOOMERANG_BROTHER_STOP_TIME)
		{
			stopStartTime = 0; 
			if (originalX + BOOMERANG_BROTHER_PATROL_DISTANCE <= x) 
			{
				vx = -BOOMERANG_BROTHER_WALKING_SPEED;
			}
			else if (x <= originalX - BOOMERANG_BROTHER_PATROL_DISTANCE)
			{
				vx = BOOMERANG_BROTHER_WALKING_SPEED;
			}
		}
		else
		{
			vx = 0; 
		}
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);

	if (isAiming && isThrowing)
	{
		DebugOut(L"[ERROR] Boomerang Brother is both aiming and throwing boomerang at the same time\n");
		return;
	}

	//if boomerang brother have all his boomerangs and mario is within distance, he will throw
	if (BoomerangCount == BOOMERANG_BROTHER_NUMBER_OF_BOOMERANG &&
		fabs(dxPlayer) <= BOOMERANG_BROTHER_THROW_DISTANCE &&
		!isAiming)
	{
		LPGAMEOBJECT boomerang = new CBoomerang(x, y);
		boomerang->SetDirection(nx);
		currentScene->Add(boomerang);
		boomerang->SetState(BOOMERANG_STATE_HELD);
		this->boomerang = boomerang;
		SetState(BOOMERANG_BROTHER_STATE_AIMING);
	}
	else if (BoomerangCount == 0 && !cooldownStartTime)
	{
		SetState(BOOMERANG_BROTHER_STATE_COOLDOWN);
	}

	if (cooldownStartTime && now - cooldownStartTime >= BOOMERANG_BROTHER_COOLDOWN_TIME)
	{
		DebugOut(L"[INFO] Boomerang Brother finished resting\n");
		cooldownStartTime = 0;
		BoomerangCount = BOOMERANG_BROTHER_NUMBER_OF_BOOMERANG; // Reset boomerang count after resting
	}


	if (isAiming)
	{
		if (now - aimStartTime >= BOOMERANG_BROTHER_AIM_TIME)
		{
			boomerang->SetState(BOOMERANG_STATE_THROWN);
			this->boomerang = nullptr;
			SetState(BOOMERANG_BROTHER_STATE_THROWING);
		}
	}
	else if (isThrowing && now - throwStartTime >= BOOMERANG_BROTHER_THROW_TIME)
	{
		if (BoomerangCount > 0) //Still have boomerang then keep throwing
		{
			LPGAMEOBJECT boomerang = new CBoomerang(x, y);
			boomerang->SetDirection(nx);
			currentScene->Add(boomerang);
			boomerang->SetState(BOOMERANG_STATE_HELD);
			this->boomerang = boomerang;
			SetState(BOOMERANG_BROTHER_STATE_AIMING);
		}	
	}

	if (boomerang)
	{
		if (isAiming && now - aimStartTime < BOOMERANG_BROTHER_AIM_TIME)
		{
			boomerang->SetPosition(x - nx * 10.f, y - 7.f);
			boomerang->SetDirection(nx);
		}
	}
}

void CBoomerangBrother::Throw()
{
	if (isAiming && isThrowing)
	{
		DebugOut(L"[ERROR] Boomerang Brother is both aiming and throwing boomerang at the same time\n");
		return; 
	}

	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());

	//if boomerang brother have all his boomerangs and mario is within distance, he will throw
	if (BoomerangCount == BOOMERANG_BROTHER_NUMBER_OF_BOOMERANG &&
		!isAiming)
	{
		LPGAMEOBJECT boomerang = new CBoomerang(x, y);
		boomerang->SetDirection(nx);
		currentScene->Add(boomerang);
		boomerang->SetState(BOOMERANG_STATE_HELD);
		this->boomerang = boomerang;
		SetState(BOOMERANG_BROTHER_STATE_AIMING);
	}
	else if (BoomerangCount == 0)
	{
		//not clear
		isAiming = false;
		isThrowing = false;
	}

	ULONGLONG now = GetTickCount64();

	if (isAiming)
	{
		if(now - aimStartTime >= BOOMERANG_BROTHER_AIM_TIME)
		{
			boomerang->SetState(BOOMERANG_STATE_THROWN);
			this->boomerang = nullptr;
			SetState(BOOMERANG_BROTHER_STATE_THROWING);
		}
	}
	else if (isThrowing && now - throwStartTime >= BOOMERANG_BROTHER_THROW_TIME)
	{
		if (BoomerangCount > 0) //Still have boomerang then keep throwing
		{
			LPGAMEOBJECT boomerang = new CBoomerang(x, y);
			boomerang->SetDirection(nx);
			currentScene->Add(boomerang);
			boomerang->SetState(BOOMERANG_STATE_HELD);
			this->boomerang = boomerang;
			SetState(BOOMERANG_BROTHER_STATE_AIMING);
		}
	}
}

void CBoomerangBrother::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CBoomerangBrother::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->obj->IsBlocking())
	{
		if (e->ny != 0)
		{
			vy = 0;
			ay = BOOMERANG_BROTHER_GRAVITY;
		}
		if (e->nx != 0)
		{
			if (e->nx > 0)
			{
				vx = -BOOMERANG_BROTHER_WALKING_SPEED;
			}
			else // Collision from the left
			{
				vx = BOOMERANG_BROTHER_WALKING_SPEED;
			}
		}
	}

	if (dynamic_cast<CBoomerang*>(e->obj))
	{
		OnCollisionWithBoomerang(e);
	}
}

void CBoomerangBrother::OnCollisionWithBoomerang(LPCOLLISIONEVENT e)
{
	CBoomerang* boomerang = dynamic_cast<CBoomerang*>(e->obj);

	if(boomerang->IsReturning())
	{
		DebugOut(L"[INFO] Boomerang Brother collided with returning boomerang\n");
		boomerang->Delete();
	}
}

void CBoomerangBrother::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - BOOMERANG_BROTHER_BBOX_WIDTH / 2;
	top = y - BOOMERANG_BROTHER_BBOX_HEIGHT / 2 - 1.f;
	right = left + BOOMERANG_BROTHER_BBOX_WIDTH;
	bottom = top + BOOMERANG_BROTHER_BBOX_HEIGHT ;
}

void CBoomerangBrother::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case BOOMERANG_BROTHER_STATE_AIMING:
	{
		isAiming = true;
		isThrowing = false;
		aimStartTime = GetTickCount64();
		break;
	}
	case BOOMERANG_BROTHER_STATE_THROWING:
	{
		DebugOut(L"[INFO] Boomerang Brother is throwing boomerang\n");
		isAiming = false;
		isThrowing = true;
		BoomerangCount--;
		throwStartTime = GetTickCount64();
		break;
	}

	case BOOMERANG_BROTHER_STATE_DIE:
	{
		DebugOut(L"[INFO] Boomerang Brother is dead\n");
		isAiming = false;
		isThrowing = false;
		die_start = GetTickCount64();
		if(boomerang)
			boomerang->Delete();
		vx = 0;
		vy = -0.1f;
		ay = 0.001f;
		break;
	}

	case BOOMERANG_BROTHER_STATE_COOLDOWN:
	{
		DebugOut(L"[INFO] Boomerang Brother is resting\n");
		isAiming = false;
		isThrowing = false;
		cooldownStartTime = GetTickCount64();
		break;
	}
	default:
		break;
	}
}

void CBoomerangBrother::Reload()
{
	originalX = x;

	ay = BOOMERANG_BROTHER_GRAVITY;
	vx = BOOMERANG_BROTHER_WALKING_SPEED;

	isAiming = false;
	isThrowing = false;
	BoomerangCount = BOOMERANG_BROTHER_NUMBER_OF_BOOMERANG;
	aimStartTime = 0;
	throwStartTime = 0;

	boomerang = nullptr;
}


