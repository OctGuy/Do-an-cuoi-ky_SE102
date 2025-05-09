#include "FireBullet.h"

void CFireBullet::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - FIRE_BULLET_BBOX_WIDTH / 2;
	t = y - FIRE_BULLET_BBOX_HEIGHT / 2;
	r = l + FIRE_BULLET_BBOX_WIDTH;
	b = t + FIRE_BULLET_BBOX_HEIGHT;
}

void CFireBullet::Render()
{
	int aniId = -1;
	if (state != FIRE_BULLET_STATE_INACTIVE)
		aniId = FIRE_BULLET_ANI;

	if (aniId != -1) {
		CAnimations* animations = CAnimations::GetInstance();
		animations->Get(aniId)->Render(x, y);
	}

	//RenderBoundingBox();
}

void CFireBullet::SetState(int state) {
	switch (state)
	{
	case FIRE_BULLET_STATE_INACTIVE:
		vx = 0;
		vy = 0;
		break;
	case FIRE_BULLET_STATE_LEFT_SHOOT_UP_NEAR:
		vx = -FIRE_BULLET_SPEED_X_NEAR;
		vy = -FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_LEFT_SHOOT_UP_FAR:
		vx = -FIRE_BULLET_SPEED_X_FAR;
		vy = -FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_LEFT_SHOOT_DOWN_FAR:
		vx = -FIRE_BULLET_SPEED_X_FAR;
		vy = FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_LEFT_SHOOT_DOWN_NEAR:
		vx = -FIRE_BULLET_SPEED_X_NEAR;
		vy = FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_RIGHT_SHOOT_UP_NEAR:
		vx = FIRE_BULLET_SPEED_X_NEAR;
		vy = -FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_RIGHT_SHOOT_UP_FAR:
		vx = FIRE_BULLET_SPEED_X_FAR;
		vy = -FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_RIGHT_SHOOT_DOWN_FAR:
		vx = FIRE_BULLET_SPEED_X_FAR;
		vy = FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_RIGHT_SHOOT_DOWN_NEAR:
		vx = FIRE_BULLET_SPEED_X_NEAR;
		vy = FIRE_BULLET_SPEED_Y;
		break;
	default:
		break;
	}
	
	CGameObject::SetState(state);
}

void CFireBullet::Reload()
{
	this->Delete();
}

void CFireBullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Same as piranha plant update

	if (state == FIRE_BULLET_STATE_INACTIVE) return;

	x += vx * dt;
	y += vy * dt;

	CCollision::GetInstance()->Process(this, dt, coObjects);
}
