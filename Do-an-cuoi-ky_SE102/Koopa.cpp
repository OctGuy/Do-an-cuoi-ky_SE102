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
		if (state == KOOPA_STATE_SHELL_REVERSE_JUMP)
			SetState(KOOPA_STATE_SHELL_REVERSE_IDLE);
		ay = KOOPA_GRAVITY; 
		platform = e->obj;  // Set platform to what Koopa is standing on
	}

	if (e->nx != 0) { 
		if (state == KOOPA_STATE_WALKING_LEFT)
			SetState(KOOPA_STATE_WALKING_RIGHT);
		else if (state == KOOPA_STATE_WALKING_RIGHT)
			SetState(KOOPA_STATE_WALKING_LEFT);
	}

	if (e->nx == 0 && e->ny == 0 && e->obj->IsBlocking())
	{
		isInWall = true;
	}

	if (state == KOOPA_STATE_SHELL_MOVE || state == KOOPA_STATE_SHELL_REVERSE_MOVE) {
		if (e->nx != 0 && e->obj->IsBlocking()) {
			if (e->nx > 0) {
				vx = KOOPA_SHELL_SPEED;
			}
			else {
				vx = -KOOPA_SHELL_SPEED;
			}
		}

		if (dynamic_cast<CQuestionBrick*>(e->obj)) {
			//DebugOut(L"Koopa collided with QuestionBrick\n");
			OnCollisionWithBrick(e);
		}	
	}
}

void CKoopa::OnCollisionWithBrick(LPCOLLISIONEVENT e) {
	CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);
	questionBrick->OnCollisionWith(e);
}

CMario* CKoopa::GetPlayer() {
	CGame* game = CGame::GetInstance();
	if (game == nullptr) return nullptr;

	CPlayScene* scene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	if (scene == nullptr) return nullptr;

	CMario* player = dynamic_cast<CMario*>(scene->GetPlayer());
	if (player == nullptr) return nullptr;

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
	else if (state == KOOPA_STATE_SHELL_REVERSE_IDLE ||
			 state == KOOPA_STATE_SHELL_REVERSE_JUMP)
		aniId = KOOPA_ANI_SHELL_REVERSE_IDLE;
	else if (state == KOOPA_STATE_SHELL_REVERSE_MOVE)
		aniId = KOOPA_ANI_SHELL_REVERSE_MOVE;
	else if (state == KOOPA_STATE_SHELL_REVERSE_SHAKING)
		aniId = KOOPA_ANI_SHELL_REVERSE_SHAKING;
	else if (state == KOOPA_STATE_DIE)
		aniId = KOOPA_ANI_DIE;
	
	if (aniId != -1)
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CKoopa::SetState(int state) {
	switch (state)
	{
	case KOOPA_STATE_WALKING_LEFT:
		DebugOut(L"[INFO] Koopa is walking left\n");
		vx = -KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_WALKING_RIGHT:
		//vy = -0.4;
		DebugOut(L"[INFO] Koopa is walking right\n");
		vx = KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_SHELL_IDLE:
		DebugOut(L"[INFO] Koopa is in shell\n");
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_MOVE:
		DebugOut(L"[INFO] Koopa is in shell and moving\n");
		ay = KOOPA_GRAVITY;				// Natural fall down when Koopa is out of platform or box
		vx = 0;							// Set vx when Collision
		break;
	case KOOPA_STATE_SHELL_SHAKING:
		DebugOut(L"[INFO] Koopa is shaking in shell\n");
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_IDLE:
		DebugOut(L"[INFO] Koopa is in shell and reverse\n");
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_MOVE:
		DebugOut(L"[INFO] Koopa is in shell and reverse moving\n");
		ay = KOOPA_GRAVITY;				// Natural fall down when Koopa is out of platform or box
		vx = 0;							// Set vx when Collision
		break;
	case KOOPA_STATE_SHELL_REVERSE_SHAKING:
		DebugOut(L"[INFO] Koopa is shaking in shell and reverse\n");
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_JUMP:
		DebugOut(L"[INFO] Koopa is in shell and reverse jump\n");
		vy = -KOOPA_DEFLECT_SPEED;      // Jump up with reverse deflect speed
		ay = KOOPA_GRAVITY;             // Apply gravity for natural arc
		vx = -vx;                       // Reverse current x velocity
		break;
	case KOOPA_STATE_DIE:
		DebugOut(L"[INFO] Koopa is dead\n");
		die_start = GetTickCount64();
		ay = KOOPA_GRAVITY;
		vx = -0.1f;
		//vy = -KOOPA_DEFLECT_SPEED;
	default:
		break;
	}

	CGameObject::SetState(state);
}

bool CKoopa::IsOnPlatform() {
	if (platform == nullptr) {
		return false;
	}

	float checkX = x + (vx > 0 ? KOOPA_BBOX_WIDTH / 2 : -KOOPA_BBOX_WIDTH / 2);
	//float checkY = y + KOOPA_BBOX_HEIGHT / 2 + 1;
	float l, t, r, b;
	platform->GetBoundingBox(l, t, r, b);

	if (checkX >= l - 8.0f && checkX <= r + 8.0f) {
		return true;
	}

	return false;
}

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CMario* player = GetPlayer();

	//DebugOut(L"[INFO] Koopa velocity: %f %f\n", vx, vy);

	 // Kiểm tra nếu Koopa đang ở trạng thái DIE
	if (state == KOOPA_STATE_DIE && player && player->GetKoopa() == this) {
		player->SetKoopa(nullptr); // Đặt Koopa của Mario thành NULL
	}

	vy += ay * dt;
	vx += ax * dt;

	ULONGLONG now = GetTickCount64();

	isInWall = false; 

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
			vy = -0.15;
			SetState(KOOPA_STATE_WALKING_LEFT);
		}
		break;
	case KOOPA_STATE_DIE:
		if (now - die_start > KOOPA_DIE_DURATION) {
			this->Delete();
		}
		break;
	}

	//DebugOut(L"KOOPA SPEED: %f \n", vx);

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CKoopa::Reload()
{
	CGameObject::Reload();
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	SetState(KOOPA_STATE_WALKING_LEFT);
	stateShellStart = -1;
	stateShakingStart = -1;
	die_start = -1;
	isHeld = false;
	platform = nullptr;
	isDeleted = false;
	isActive = true;
}
