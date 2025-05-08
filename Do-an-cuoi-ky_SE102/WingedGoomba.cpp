#include "WingedGoomba.h"

CWingedGoomba::CWingedGoomba(float x, float y) : CEnemy (x, y) {
	this->ax = 0;
	this->ay = GOOMBA_WING_GRAVITY;
	die_start = -1;
	isOnPlatform = false;
	bounceCount = 0;
	SetState(GOOMBA_WING_STATE_WALKING);
}

void CWingedGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	if (state == GOOMBA_WING_STATE_DIE) {
		left = x - GOOMBA_BASE_BBOX_WIDTH / 2;
		top = y - GOOMBA_BBOX_HEIGHT_DIE / 2;
		right = left + GOOMBA_BASE_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
	}
	else if (state == GOOMBA_WING_STATE_BOUNCE || state == GOOMBA_WING_STATE_FLY) {
		left = x - GOOMBA_WING_BBOX_WIDTH / 2;
		top = y - GOOMBA_WING_BBOX_HEIGHT / 2;
		right = left + GOOMBA_WING_BBOX_WIDTH;
		bottom = top + GOOMBA_WING_BBOX_HEIGHT;
	}
	else {
		left = x - GOOMBA_BASE_BBOX_WIDTH / 2;
		top = y - GOOMBA_BASE_BBOX_HEIGHT / 2;
		right = left + GOOMBA_BASE_BBOX_WIDTH;
		bottom = top + GOOMBA_BASE_BBOX_HEIGHT;
	}
}

void CWingedGoomba::OnNoCollision(DWORD dt) {
	x += vx * dt;
	y += vy * dt;
}

void CWingedGoomba::OnCollisionWith(LPCOLLISIONEVENT e) {
	if (e->ny < 0 && e->obj->IsBlocking()) { // Stand on platform
		vy = 0;
		ay = GOOMBA_WING_GRAVITY;
		isOnPlatform = true;
	}
}

void CWingedGoomba::Render() {
	int aniId = -1;

	if (state == GOOMBA_WING_STATE_DIE) 
		aniId = GOOMBA_WING_ANI_DIE;
	else if (state == GOOMBA_WING_STATE_DIE_REVERSE) 
		aniId = GOOMBA_WING_ANI_DIE_REVERSE;
	else if (state == GOOMBA_WING_STATE_FLY)
		aniId = GOOMBA_WING_ANI_FLY;
	else if (state == GOOMBA_WING_STATE_WALKING)
		aniId = GOOMBA_WING_ANI_WALKING;
	else if (state == GOOMBA_WING_STATE_BOUNCE) {
		if (isOnPlatform)
			aniId = GOOMBA_WING_ANI_CLOSE_WING;
		else
			aniId = GOOMBA_WING_ANI_OPEN_WING;
	}

	if (aniId != -1)
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	RenderBoundingBox();
}

void CWingedGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	vx += ax * dt;
	vy += ay * dt;

		//// Handle bouncing logic
		//if (state == GOOMBA_WING_STATE_BOUNCE && isOnPlatform) {
		//	if (bounceCount < 4) {
		//		vy = -0.1f;
		//		vx = -0.1f;
		//		bounceCount++;
		//		isOnPlatform = false;
		//	}
		//	else SetState(GOOMBA_WING_STATE_BOUNCE);
		//}

	if ((state == GOOMBA_WING_STATE_DIE && GetTickCount64() - die_start > GOOMBA_DIE_TIMEOUT)
		|| (state == GOOMBA_WING_STATE_DIE_REVERSE && GetTickCount64() - die_start > GOOMBA_DIE_REVERSE_TIMEOUT)) {
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CWingedGoomba::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case GOOMBA_WING_STATE_FLY:
		
		break;
	case GOOMBA_WING_STATE_WALKING:
		vx = -GOOMBA_WING_WALKING_SPEED;
		ay = GOOMBA_WING_GRAVITY;
		break;
	case GOOMBA_WING_STATE_BOUNCE:
		vx = 0;
		ax = 0;
		bounceCount = 0;
		break;
	case GOOMBA_WING_STATE_DIE:
		DebugOut(L"GOOMBA WING DIE\n");
		die_start = GetTickCount64();
		y += (GOOMBA_BASE_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2;
		vx = 0;
		vy = 0;
		ay = 0;
		break;
	case GOOMBA_WING_STATE_DIE_REVERSE:
		DebugOut(L"GOOMBA WING DIE REVERSE\n");
		die_start = GetTickCount64();
		vx = 0;
		vy = -GOOMBA_WING_DEFLECT_SPEED;
		ay = GOOMBA_WING_GRAVITY;
		break;
	default:
		break;
	}
}
