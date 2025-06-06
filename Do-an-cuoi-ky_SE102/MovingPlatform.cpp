#include "MovingPlatform.h"
#include "Mario.h"
#include <vector>

void CMovingPlatform::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMovingPlatform::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CMovingPlatform::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (dynamic_cast<CMario*>(e->obj))
	{
		OnCollisionWithMario(e);
	}
}

void CMovingPlatform::OnCollisionWithMario(LPCOLLISIONEVENT e)
{
	if (e->ny > 0) {
		DebugOut(L"[INFO] Collided from top\n");
		ay = MOVING_PLATFORM_GRAVITY; // Reset gravity to default
		vx = 0;
		vy = MOVING_PLATFORM_VERTICAL_SPEED;
	}
}

void CMovingPlatform::Reload()
{
	CGameObject::Reload();
	vx = -MOVING_PLATFORM_HORIZONTAL_SPEED;
}

