#include "Boomerang.h"

void CBoomerang::Render()
{
	int aniId = -1;
	if (isThrown)
	{
		if (nx == 1)
			aniId = ID_ANI_BOOMERANG_FLYING_RIGHT;
		else
			aniId = ID_ANI_BOOMERANG_FLYING_LEFT;
	}
	else
	{
		if (nx == 1)
			aniId = ID_ANI_BOOMERANG_HELD_RIGHT;
		else
			aniId = ID_ANI_BOOMERANG_HELD_LEFT;
	}

	if (aniId != -1)
	{
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	}
	else
	{
		DebugOut(L"[ERROR] Boomerang animation ID is not set\n");
	}
	RenderBoundingBox();
}

void CBoomerang::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vx += ax * dt; 
	vy += ay * dt;

	if(signbit(vx) == signbit(ax) && !isReturning && vx && ax)
		isReturning = true;

	//if (vy >= BOOMERANG_FLYING_SPEED_Y/2)
	//	vy = 0;
	if (vy >= 0.03f)
		vy = 0;

	x += vx * dt;
	y += vy * dt;

	DebugOut(L"[INFO] Boomerang Update: %f %f\n", vx, vy);

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CBoomerang::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - BOOMERANG_BBOX_WIDTH / 2;
	top = y - BOOMERANG_BBOX_HEIGHT / 2;
	right = left + BOOMERANG_BBOX_WIDTH;
	bottom = top + BOOMERANG_BBOX_HEIGHT;
}

void CBoomerang::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{

	case BOOMERANG_STATE_THROWN:
		isThrown = true;
		vx = (nx > 0) ? BOOMERANG_FLYING_SPEED : -BOOMERANG_FLYING_SPEED;
		ax = (nx > 0) ? -0.0001f : 0.0001f;
		vy = -BOOMERANG_FLYING_SPEED_Y;
		ay = 0.00015f;
		break;

	case BOOMERANG_STATE_HELD:
		isThrown = false;
		vx = 0;
		break;

	default:
		break;
	}
}




