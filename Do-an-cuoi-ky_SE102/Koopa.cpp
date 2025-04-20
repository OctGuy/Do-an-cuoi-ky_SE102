#include "Koopa.h"

void CKoopa::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT)
	{
		l = x - KOOPA_BBOX_WIDTH / 2;
		t = y - KOOPA_BBOX_HEIGHT / 2;
		r = l + KOOPA_BBOX_WIDTH;
		b = t + KOOPA_BBOX_HEIGHT;
	}
	else
	{
		l = x - KOOPA_SHELL_BBOX_WIDTH / 2;
		t = y - KOOPA_SHELL_BBOX_HEIGHT / 2;
		r = l + KOOPA_SHELL_BBOX_WIDTH;
		b = t + KOOPA_SHELL_BBOX_HEIGHT;
	}
}

void CKoopa::OnNoCollision(DWORD dt) {
	x += vx * dt;
	y += vy * dt;
}

void CKoopa::OnCollisionWith(LPCOLLISIONEVENT e) {
	if (!e->obj->IsBlocking()) return;
	//if (e->ny != 0) {
	//	vy = 0;
	//	ay = KOOPA_GRAVITY; // Reset gravity to default
	//}
	//else if (e->nx != 0) {
	//	vx = -vx; // Reverse direction on collision with left or right
	//}

	if (e->ny < 0) { // Stand on platform
		vy = 0;
		ay = KOOPA_GRAVITY; 
		if (platform == NULL && dynamic_cast<CBoxPlatform*>(e->obj)) {
			platform = dynamic_cast<CBoxPlatform*>(e->obj);
		}
	}

	if (e->nx != 0) { 
		if (state == KOOPA_STATE_WALKING_LEFT)
			SetState(KOOPA_STATE_WALKING_RIGHT);
		else if (state == KOOPA_STATE_WALKING_RIGHT)
			SetState(KOOPA_STATE_WALKING_LEFT);
	}
}

void CKoopa::Render() {
	int aniId = -1;
	if (state == KOOPA_STATE_WALKING_LEFT)
		aniId = KOOPA_ANI_WALKING_LEFT;
	else if (state == KOOPA_STATE_WALKING_RIGHT)
		aniId = KOOPA_ANI_WALKING_RIGHT;
	else if (state == KOOPA_STATE_SHELL_IDLE)
		aniId = KOOPA_ANI_SHELL_IDLE;
	//else if (state == KOOPA_STATE_SHELL_MOVE)
		//aniId = KOOPA_ANI_SHELL_MOVE;
	else if (state == KOOPA_STATE_SHELL_SHAKING)
		aniId = KOOPA_ANI_SHELL_SHAKING;
	else if (state == KOOPA_STATE_SHELL_REVERSE_IDLE)
		aniId = KOOPA_ANI_SHELL_REVERSE_IDLE;
	//else if (state == KOOPA_STATE_SHELL_REVERSE_MOVE)
		//aniId = KOOPA_ANI_SHELL_REVERSE_MOVE;
	else if (state == KOOPA_STATE_SHELL_REVERSE_SHAKING)
		aniId = KOOPA_ANI_SHELL_REVERSE_SHAKING;
	
	if (aniId != -1)
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	RenderBoundingBox();
}

void CKoopa::SetState(int state) {
	switch (state)
	{
	case KOOPA_STATE_WALKING_LEFT:
		vx = -KOOPA_WALKING_SPEED;
		ay = KOOPA_GRAVITY; // Reset gravity to default
		break;
	case KOOPA_STATE_WALKING_RIGHT:
		ay = KOOPA_GRAVITY; // Reset gravity to default
		vx = KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_SHELL_IDLE:
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	/*case KOOPA_STATE_SHELL_MOVE:
		//vx = (nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED;
		//break;*/
	case KOOPA_STATE_SHELL_SHAKING:
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_IDLE:
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	/*case KOOPA_STATE_SHELL_REVERSE_MOVE:
		//vx = (nx > 0) ? -KOOPA_SHELL_SPEED : KOOPA_SHELL_SPEED;
		//break;*/
	case KOOPA_STATE_SHELL_REVERSE_SHAKING:
		vx = 0;
		break;
	default:
		break;
	}
	CGameObject::SetState(state);
}

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	vy += ay * dt;
	vx += ax * dt;

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}
