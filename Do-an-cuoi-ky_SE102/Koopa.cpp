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

	if (e->ny < 0) { // Stand on platform
		vy = 0;
		ay = KOOPA_GRAVITY; 
		platform = e->obj;  // Set platform to what Koopa is standing on
	}

	if (e->nx != 0) { 
		if (state == KOOPA_STATE_WALKING_LEFT)
			SetState(KOOPA_STATE_WALKING_RIGHT);
		else if (state == KOOPA_STATE_WALKING_RIGHT)
			SetState(KOOPA_STATE_WALKING_LEFT);
	}

	if (state == KOOPA_STATE_SHELL_MOVE || state == KOOPA_STATE_SHELL_REVERSE_MOVE) {
		if (e->nx != 0 && e->obj->IsBlocking()) {
			vx = -vx;
		}

		if (dynamic_cast<CQuestionBrick*>(e->obj)) {
			//DebugOut(L"Koopa collided with QuestionBrick\n");
			OnCollisionWithBrick(e);
		}
			
	}
	//DebugOut(L"Koopa is on platform: %d\n", isOnPlatform);
}

void CKoopa::OnCollisionWithBrick(LPCOLLISIONEVENT e) {
	CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);
	questionBrick->OnCollisionWith(e);
}

CMario* CKoopa::GetPlayer() {
	CGame* game = CGame::GetInstance();
	if (game == NULL) return NULL;

	CPlayScene* scene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	if (scene == NULL) return NULL;

	CMario* player = dynamic_cast<CMario*>(scene->GetPlayer());
	if (player == NULL) return NULL;

	return player;
}

void CKoopa::Render() {
	int aniId = -1;

	if (state == KOOPA_STATE_WALKING_LEFT)
		aniId = KOOPA_ANI_WALKING_LEFT;
	else if (state == KOOPA_STATE_WALKING_RIGHT)
		aniId = KOOPA_ANI_WALKING_RIGHT;
	else if (state == KOOPA_STATE_SHELL_IDLE)
		aniId = KOOPA_ANI_SHELL_IDLE;
	else if (state == KOOPA_STATE_SHELL_MOVE)
		aniId = KOOPA_ANI_SHELL_MOVE;
	else if (state == KOOPA_STATE_SHELL_SHAKING)
		aniId = KOOPA_ANI_SHELL_SHAKING;
	else if (state == KOOPA_STATE_SHELL_REVERSE_IDLE)
		aniId = KOOPA_ANI_SHELL_REVERSE_IDLE;
	else if (state == KOOPA_STATE_SHELL_REVERSE_MOVE)
		aniId = KOOPA_ANI_SHELL_REVERSE_MOVE;
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
		break;
	case KOOPA_STATE_WALKING_RIGHT:
		//vy = -0.4;
		vx = KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_SHELL_IDLE:
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_MOVE:
		ay = KOOPA_GRAVITY;				// Natural fall down when Koopa is out of platform or box
		vx = 0;							// Set vx when Collision
		break;
	case KOOPA_STATE_SHELL_SHAKING:
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_IDLE:
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_MOVE:
		ay = KOOPA_GRAVITY;				// Natural fall down when Koopa is out of platform or box
		vx = 0;							// Set vx when Collision
		break;
	case KOOPA_STATE_SHELL_REVERSE_SHAKING:
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	default:
		break;
	}

	CGameObject::SetState(state);
}

bool CKoopa::IsOnPlatform() {
	if (platform == NULL) {
		return false;
	}

	float checkX = x + (vx > 0 ? KOOPA_BBOX_WIDTH / 2 : -KOOPA_BBOX_WIDTH / 2);
	//float checkY = y + KOOPA_BBOX_HEIGHT / 2 + 1;
	float l, t, r, b;
	platform->GetBoundingBox(l, t, r, b);

	if (checkX >= l && checkX <= r) {
		return true;
	}

	return false;
}

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CMario* player = GetPlayer();

	//DebugOut(L"[INFO] Koopa velocity: %f %f\n", vx, vy);

	vy += ay * dt;
	vx += ax * dt;

	ULONGLONG now = GetTickCount64();

	bool isOnPlatform = IsOnPlatform();
	if (!isOnPlatform) {
		if (state == KOOPA_STATE_WALKING_LEFT)
			SetState(KOOPA_STATE_WALKING_RIGHT);
		else if (state == KOOPA_STATE_WALKING_RIGHT)
			SetState(KOOPA_STATE_WALKING_LEFT);
	}

	switch (state) 
	{
	case KOOPA_STATE_SHELL_IDLE:
	case KOOPA_STATE_SHELL_REVERSE_IDLE:
		if (now - stateShellStart > KOOPA_SHELL_DURATION) {
			SetState(state == KOOPA_STATE_SHELL_IDLE
				? KOOPA_STATE_SHELL_SHAKING
				: KOOPA_STATE_SHELL_REVERSE_SHAKING);
		}
		break;

	case KOOPA_STATE_SHELL_SHAKING:
	case KOOPA_STATE_SHELL_REVERSE_SHAKING:
		if (now - stateShakingStart > KOOPA_SHELL_SHAKING_DURATION) {
			//DebugOut(L"[INFO] Koopa is out of shell\n");
			vy = -0.4;
			SetState(KOOPA_STATE_WALKING_LEFT);
		}
		break;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}
