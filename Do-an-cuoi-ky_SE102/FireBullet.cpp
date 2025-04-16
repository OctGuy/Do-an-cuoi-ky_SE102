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
	int aniId = FIRE_BULLET_ANI_INACTIVE;
	if (state != FIRE_BULLET_STATE_INACTIVE)
		aniId = FIRE_BULLET_ANI;

	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}

void CFireBullet::SetState(int state) {
	this->state = state;

	switch (state)
	{
	case FIRE_BULLET_STATE_INACTIVE:
		vx = 0;
		vy = 0;
		break;
	case FIRE_BULLET_STATE_LEFT_SHOOT_HIGH:
		vx = -FIRE_BULLET_SPEED_X_HIGH;
		vy = -FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_LEFT_SHOOT_FAR:
		vx = -FIRE_BULLET_SPEED_X_FAR;
		vy = FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_LEFT_SHOOT_LOW:
		vx = -FIRE_BULLET_SPEED_X_LOW;
		vy = FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_RIGHT_SHOOT_HIGH:
		vx = FIRE_BULLET_SPEED_X_HIGH;
		vy = -FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_RIGHT_SHOOT_FAR:
		vx = FIRE_BULLET_SPEED_X_FAR;
		vy = FIRE_BULLET_SPEED_Y;
		break;
	case FIRE_BULLET_STATE_RIGHT_SHOOT_LOW:
		vx = FIRE_BULLET_SPEED_X_LOW;
		vy = FIRE_BULLET_SPEED_Y;
		break;
	}

	//CGameObject::SetState(state);
}

void CFireBullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Same as piranha plant update

	if (state == FIRE_BULLET_STATE_INACTIVE) return;

	x += vx * dt;
	y += vy * dt;

	if (y > 222.0f) { // bullet fall of the ground
		SetState(FIRE_BULLET_STATE_INACTIVE);
		return;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}
