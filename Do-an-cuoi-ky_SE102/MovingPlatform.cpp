#include "MovingPlatform.h"
#include "Mario.h"
#include <vector>

void CMovingPlatform::Render()
{
	CPlatform::Render();
}

void CMovingPlatform::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	//DebugOut(L"[INFO] MovingPlatform Update: %f %f\n", vx, vy);

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
		vx = 0;
		vy = MOVING_PLATFORM_VERTICAL_SPEED;
	}
}

void CMovingPlatform::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	CPlatform::GetBoundingBox(l, t, r, b);
}

void CMovingPlatform::Reload()
{
	//DebugOut(L"[INFO] Reload MovingPlatform\n");
	CGameObject::Reload();
	vx = -MOVING_PLATFORM_HORIZONTAL_SPEED;
}

