#include "ParaTroopa.h"

CMario* CParaTroopa::GetPlayer() {
	CGame* game = CGame::GetInstance();
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());
	return mario;
}

void CParaTroopa::Render() {
	int aniId = -1;

	if (state == PARATROOPA_STATE_WALKING_LEFT) 
		aniId = PARATROOPA_ANI_WALKING_LEFT;
	else if (state == PARATROOPA_STATE_WALKING_RIGHT) 
		aniId = PARATROOPA_ANI_WALKING_RIGHT;
	else if (state == PARATROOPA_STATE_BOUNCE_LEFT)
		aniId = PARATROOPA_ANI_BOUNCE_LEFT;
	else if (state == PARATROOPA_STATE_BOUNCE_RIGHT) 
		aniId = PARATROOPA_ANI_BOUNCE_RIGHT;
	else if (state == PARATROOPA_STATE_SHELL_IDLE) 
		aniId = PARATROOPA_ANI_SHELL_IDLE;
	else if (state == PARATROOPA_STATE_SHELL_MOVE) 
		aniId = PARATROOPA_ANI_SHELL_MOVE;
	else if (state == PARATROOPA_STATE_SHELL_SHAKING) 
		aniId = PARATROOPA_ANI_SHELL_SHAKING;
	else if (state == PARATROOPA_STATE_SHELL_REVERSE_IDLE
			|| state == PARATROOPA_STATE_SHELL_REVERSE_JUMP) 
		aniId = PARATROOPA_ANI_SHELL_REVERSE_IDLE;
	else if (state == PARATROOPA_STATE_SHELL_REVERSE_MOVE) 
		aniId = PARATROOPA_ANI_SHELL_REVERSE_MOVE;
	else if (state == PARATROOPA_STATE_SHELL_REVERSE_SHAKING) 
		aniId = PARATROOPA_ANI_SHELL_REVERSE_SHAKING;
	else if (state == PARATROOPA_STATE_DIE) 
		aniId = PARATROOPA_ANI_DIE;
	
	if (aniId != -1)
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CParaTroopa::GetBoundingBox(float& l, float& t, float& r, float& b) {
	if (state == PARATROOPA_STATE_WALKING_LEFT 
		|| state == PARATROOPA_STATE_WALKING_RIGHT
		|| state == PARATROOPA_STATE_BOUNCE_LEFT
		|| state == PARATROOPA_STATE_BOUNCE_RIGHT) {
		l = x - PARATROOPA_BBOX_WIDTH / 2;
		t = y - PARATROOPA_BBOX_HEIGHT / 2;
		r = l + PARATROOPA_BBOX_WIDTH;
		b = t + PARATROOPA_BBOX_HEIGHT;
	}
	else {
		l = x - PARATROOPA_SHELL_BBOX_WIDTH / 2;
		t = y - PARATROOPA_SHELL_BBOX_HEIGHT / 2;
		r = l + PARATROOPA_SHELL_BBOX_WIDTH;
		b = t + PARATROOPA_SHELL_BBOX_HEIGHT;
	}
}

void CParaTroopa::OnNoCollision(DWORD dt) {
	x += vx * dt;
	y += vy * dt;
}

void CParaTroopa::OnCollisionWith(LPCOLLISIONEVENT e) {
	if (dynamic_cast<CParaTroopa*>(e->obj))
		OnCollisionWithParaTroopa(e);
	else if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);

	if (e->ny < 0) { // standing on platform
		vy = 0;

		if (state == PARATROOPA_STATE_SHELL_REVERSE_JUMP)
			SetState(PARATROOPA_STATE_SHELL_REVERSE_IDLE); // Mario tail attack

		if ((state == PARATROOPA_STATE_BOUNCE_LEFT || state == PARATROOPA_STATE_BOUNCE_RIGHT)
			&& e->obj->IsBlocking()) {
			vy = -PARATROOPA_BOUNCE_SPEED; //  bounce up 
		}

		ay = PARATROOPA_GRAVITY;
	}

	if (e->nx != 0 && e->obj->IsBlocking()) {
		if (state == PARATROOPA_STATE_WALKING_LEFT 
			|| state == PARATROOPA_STATE_WALKING_RIGHT) 
			SetState(e->nx > 0 
				? PARATROOPA_STATE_WALKING_RIGHT 
				: PARATROOPA_STATE_WALKING_LEFT);
		else if (state == PARATROOPA_STATE_BOUNCE_LEFT 
				|| state == PARATROOPA_STATE_BOUNCE_RIGHT)
			SetState(e->nx > 0 
				? PARATROOPA_STATE_BOUNCE_RIGHT 
				: PARATROOPA_STATE_BOUNCE_LEFT);
		else if (state == PARATROOPA_STATE_SHELL_MOVE 
				|| state == PARATROOPA_STATE_SHELL_REVERSE_MOVE) 
			vx = e->nx > 0 
				? PARATROOPA_SHELL_SPEED 
				: -PARATROOPA_SHELL_SPEED;
	}

	if (e->nx == 0 && e->ny == 0 && e->obj->IsBlocking()) isInWall = true;

	if (dynamic_cast<CShinyBrick*>(e->obj))
		OnCollisionWithShinyBrick(e);
	else if (dynamic_cast<CQuestionBrick*>(e->obj))
		OnCollisionWithBrick(e);
}

void CParaTroopa::OnCollisionWithBrick(LPCOLLISIONEVENT e) {
	CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);

	if (e->nx != 0 && state == PARATROOPA_STATE_SHELL_MOVE || state == PARATROOPA_STATE_SHELL_REVERSE_MOVE)
		questionBrick->OnCollisionWith(e);
	else if (e->ny < 0)
	{
		//DebugOut(L"[INFO] Koopa hit QuestionBrick from above\n");
		questionBrick->SetKoopa(this);
	}
}

void CParaTroopa::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {
	CMario* mario = GetPlayer();
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

	if (koopa) {
		if (koopa->GetIsHeld()) {
			//DebugOut(L"Koopa is collided with ParaTroopa when Mario hold\n");
			SetState(PARATROOPA_STATE_DIE);
			koopa->SetState(KOOPA_STATE_DIE);
			mario->AddPoint(100, e);
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE) {
			//DebugOut(L"Koopa is collided with ParaTroopa when Mario kick\n");
			SetState(PARATROOPA_STATE_DIE);
			mario->AddPoint(100, e);
		}

		if (this->GetIsHeld()) {
			//DebugOut(L"Koopa is collided with ParaTroopa when Mario hold\n");
			SetState(PARATROOPA_STATE_DIE);
			koopa->SetState(KOOPA_STATE_DIE);
			mario->AddPoint(100, e);
		}
		else if (this->GetState() == PARATROOPA_STATE_SHELL_MOVE
			|| this->GetState() == PARATROOPA_STATE_SHELL_REVERSE_MOVE) {
			//DebugOut(L"Koopa is collided with ParaTroopa when Mario kick\n");
			koopa->SetState(KOOPA_STATE_DIE);
			mario->AddPoint(100, e);
		}
	}
}

void CParaTroopa::OnCollisionWithParaTroopa(LPCOLLISIONEVENT e) {
	CMario* mario = GetPlayer();
	CParaTroopa* paraTroopa = dynamic_cast<CParaTroopa*>(e->obj);

	if (paraTroopa) {
		// ParaTroopa collides with another ParaTroopa
		if (paraTroopa->GetIsHeld()) {
			//DebugOut(L"[INFO] ParaTroopa is held by Mario collide with another ParaTroopa\n");
			paraTroopa->SetState(PARATROOPA_STATE_DIE);
			this->SetState(PARATROOPA_STATE_DIE);
			mario->AddPoint(100);
		}
		else if (paraTroopa->GetState() == PARATROOPA_STATE_SHELL_MOVE
			|| paraTroopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_MOVE) {
			//DebugOut(L"[INFO] ParaTroopa moving shell collides with another ParaTroopa\n");
			this->SetState(PARATROOPA_STATE_DIE);
			mario->AddPoint(100);
		}
	}
}

void CParaTroopa::OnCollisionWithShinyBrick(LPCOLLISIONEVENT e) {
	CShinyBrick* shinyBrick = dynamic_cast<CShinyBrick*>(e->obj);
	if (e->nx != 0)
		shinyBrick->Activate();
}

void CParaTroopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CMario* player = GetPlayer();

	if (state == PARATROOPA_STATE_DIE && player && player->GetKoopa() == this) {
		player->SetKoopa(nullptr);
	}
	
	vy += ay * dt;
	vx += ax * dt;

	ULONGLONG now = GetTickCount64();

	isInWall = false;

	switch (state)
	{
	case PARATROOPA_STATE_SHELL_IDLE:
	case PARATROOPA_STATE_SHELL_REVERSE_IDLE:
		if (now - stateShellStart > PARATROOPA_SHELL_DURATION) {
			SetState(state == PARATROOPA_STATE_SHELL_IDLE
				? PARATROOPA_STATE_SHELL_SHAKING
				: PARATROOPA_STATE_SHELL_REVERSE_SHAKING);
		}
		break;

	case PARATROOPA_STATE_SHELL_SHAKING:
	case PARATROOPA_STATE_SHELL_REVERSE_SHAKING:
		if (now - stateShakingStart > PARATROOPA_SHELL_SHAKING_DURATION) {
			vy = -0.15;
			SetState(PARATROOPA_STATE_WALKING_LEFT);
		}
		break;
	case PARATROOPA_STATE_DIE:
		if (now - die_start > PARATROOPA_DIE_DURATION) {
			this->Delete();
		}
		break;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CParaTroopa::SetState(int state) {
	switch (state) {
	case PARATROOPA_STATE_WALKING_LEFT:
		//DebugOut(L"[INFO] Paratroopa is walking left\n");
		vx = -PARATROOPA_WALKING_SPEED;
		break;
	case PARATROOPA_STATE_WALKING_RIGHT:
		//DebugOut(L"[INFO] Paratroopa is walking right\n");
		vx = PARATROOPA_WALKING_SPEED;
		break;
	case PARATROOPA_STATE_BOUNCE_LEFT:
		//DebugOut(L"[INFO] Paratroopa is bounce left\n");
		vx = -PARATROOPA_WALKING_SPEED;
		vy = -PARATROOPA_BOUNCE_SPEED;
		ay = PARATROOPA_GRAVITY; 
		break;
	case PARATROOPA_STATE_BOUNCE_RIGHT:
		//DebugOut(L"[INFO] Paratroopa is bounce right\n");
		vx = PARATROOPA_WALKING_SPEED;
		vy = -PARATROOPA_BOUNCE_SPEED;
		ay = PARATROOPA_GRAVITY; 
		break;
	case PARATROOPA_STATE_SHELL_IDLE:
		//DebugOut(L"[INFO] Paratroopa is shell idle\n");
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case PARATROOPA_STATE_SHELL_MOVE:
		//DebugOut(L"[INFO] Paratroopa is in shell move\n");
		ay = PARATROOPA_GRAVITY;				
		vx = 0;							
		break;
	case PARATROOPA_STATE_SHELL_SHAKING:
		//DebugOut(L"[INFO] Paratroopa is shell shaking\n");
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	case PARATROOPA_STATE_SHELL_REVERSE_IDLE:
		//DebugOut(L"[INFO] Paratroopa is shell reverse idle\n");
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case PARATROOPA_STATE_SHELL_REVERSE_MOVE:
		//DebugOut(L"[INFO] Paratroopa is shell reverse move\n");
		ay = KOOPA_GRAVITY;				
		vx = 0;							
		break;
	case PARATROOPA_STATE_SHELL_REVERSE_SHAKING:
		//DebugOut(L"[INFO] Paratroopa is shell reverse shaking\n");
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	case PARATROOPA_STATE_SHELL_REVERSE_JUMP:
		//DebugOut(L"[INFO] Paratroopa is shell reverse jump\n");
		vy = -PARATROOPA_DEFLECT_SPEED;      
		ay = PARATROOPA_GRAVITY;             
		vx = -vx;                       
		break;
	case PARATROOPA_STATE_DIE:
		//DebugOut(L"[INFO] Paratroopa is dead\n");
		die_start = GetTickCount64();
		ay = PARATROOPA_GRAVITY;
		vx = -0.1f;
	default:
		break;
	}

	CGameObject::SetState(state);
}

void CParaTroopa::Reload() {
	CGameObject::Reload();
	this->ax = 0;
	this->ay = PARATROOPA_GRAVITY;

	CMario* mario = GetPlayer();
	float mX, mY;
	mario->GetPosition(mX, mY);

	SetState((x0 == ORIGINAL_X_PARATROOPA_WALKING && y0 == ORIGINAL_Y_PARATROOPA_WALKING)
		? (x < mX ? PARATROOPA_STATE_WALKING_RIGHT : PARATROOPA_STATE_WALKING_LEFT)
		: (x < mX ? PARATROOPA_STATE_BOUNCE_RIGHT : PARATROOPA_STATE_BOUNCE_LEFT));

	stateShellStart = -1;
	stateShakingStart = -1;
	die_start = -1;
	isHeld = false;
	isInWall = false;
	isActive = true;
}

