#include "BoomerangBrother.h"
#include "Mario.h"
#include "PlayScene.h"
void CBoomerangBrother::Render()
{
	int aniId = -1;
	if (nx == 1)
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

	RenderBoundingBox();
}

void CBoomerangBrother::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt; 

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

	if (fabs(dxPlayer) <= BOOMERANG_BROTHER_THROW_DISTANCE)
		Throw();

	ULONGLONG now = GetTickCount64();

	//Logic for patrolling, if hit left edge, wait, move right, if hit right edge, wait, move left
	if(fabs(x - originalX) >= BOOMERANG_BROTHER_PATROL_DISTANCE)
	{
		if(!waitStartTime)
			waitStartTime = now;

		if (now - waitStartTime >= BOOMERANG_BROTHER_WAIT_TIME)
		{
			waitStartTime = 0; 
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
}

void CBoomerangBrother::Throw()
{
	//Throwing and aiming logic
	if (isAiming && isThrowing)
	{
		DebugOut(L"[ERROR] Boomerang Brother is both aiming and throwing boomerang at the same time\n");
		return; // Prevents further processing if both states are active
	}

	//if boomerang brother have all his boomerangs and mario is within distance, he will throw
	if (BoomerangCount == BOOMERANG_BROTHER_NUMBER_OF_BOOMERANG &&
		!isAiming)
	{
		SetState(BOOMERANG_BROTHER_STATE_AIMING);
	}
	else if (BoomerangCount == 0)
	{
		//not clear
		isAiming = false;
		isThrowing = false;
	}

	ULONGLONG now = GetTickCount64();

	if (isAiming && now - aimStartTime >= BOOMERANG_BROTHER_AIM_TIME)
	{
		SetState(BOOMERANG_BROTHER_STATE_THROWING);
	}
	else if (isThrowing && now - throwStartTime >= BOOMERANG_BROTHER_THROW_TIME)
	{
		if (BoomerangCount > 0) //Still have boomerang then keep throwing
		{
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
				//DebugOut(L"[INFO] Boomerang Brother collided from the right\n");
				vx = -BOOMERANG_BROTHER_WALKING_SPEED;
			}
			else // Collision from the left
			{
				//DebugOut(L"[INFO] Boomerang Brother collided from the left\n");
				vx = BOOMERANG_BROTHER_WALKING_SPEED;
			}
		}
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
		//if (BoomerangCount >= BOOMERANG_BROTHER_NUMBER_OF_BOOMERANG)
		//{
		//	BoomerangCount = 0; // Reset count after throwing all boomerangs
		//	isThrowing = false; // Stop throwing after all boomerangs are used
		//}
		break;
	}
	default:
		break;
	}
}


