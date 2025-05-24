#include "Goomba.h"
#include "WingedGoomba.h"
#include "ParaTroopa.h"

CGoomba::CGoomba(float x, float y) :CGameObject(x, y)
{
	this->ax = 0;
	this->ay = 0;
	die_start = -1;
	SetState(GOOMBA_STATE_WALKING);
}

CMario* CGoomba::GetPlayer() {
	CGame* game = CGame::GetInstance();
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(playScene->GetPlayer());

	return mario;
}

void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == GOOMBA_STATE_DIE)
	{
		left = x - GOOMBA_BBOX_WIDTH / 2;
		top = y - GOOMBA_BBOX_HEIGHT_DIE / 2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
	}
	else
	{
		left = x - GOOMBA_BBOX_WIDTH / 2;
		top = y - GOOMBA_BBOX_HEIGHT / 2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT;
	}
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if (((state == GOOMBA_STATE_DIE) && (GetTickCount64() - die_start > GOOMBA_DIE_TIMEOUT))
		|| (state == GOOMBA_STATE_DIE_REVERSE) && (GetTickCount64() - die_start > GOOMBA_DIE_REVERSE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}

	//CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CGoomba::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 || e->nx != 0)
	{
		if (e->ny != 0 && e->obj->IsBlocking())
		{
			vy = 0;
			ay = GOOMBA_GRAVITY; // Reset gravity to default
		}
		else if (e->nx != 0)
		{
			if (e->obj->IsBlocking()) {
				DebugOut(L"GOOMBA COLLISION WITH BLOCKING OBJECT\n");
				if (e->nx > 0)
					vx = GOOMBA_WALKING_SPEED;
				else
					vx = -GOOMBA_WALKING_SPEED;
			}
		}
	}
	else
		if (dynamic_cast<CKoopa*>(e->obj))
			OnCollisionWithKoopa(e);
}

void CGoomba::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {
	CMario* mario = GetPlayer();
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
	CKoopa* koopaHeldByMario = dynamic_cast<CKoopa*>(mario->GetKoopa());

	if (koopa) {
		if (koopa->GetIsHeld()) {
			DebugOut(L"Koopa is collided with Goomba when Mario hold\n");
			SetState(GOOMBA_STATE_DIE_REVERSE);
			koopa->SetState(KOOPA_STATE_DIE);
			mario->AddPoint(100, e);
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE) {
			DebugOut(L"Koopa is collided with Goomba when Mario kick\n");
			SetState(GOOMBA_STATE_DIE_REVERSE);
			mario->AddPoint(100, e);
		}
	}
}

void CGoomba::Render()
{
	//Keep the goomba in suspend aniamtion when game is pause or freeze
	CGame* game = CGame::GetInstance();
	if (game->IsPaused() || game->IsTimeFrozen()) {
		CSprites* s = CSprites::GetInstance();
		s->Get(currentAniId)->Draw(x, y);
		return;
	}

	//Render Normally
	int aniId = ID_ANI_GOOMBA_WALKING;
	if (state == GOOMBA_STATE_DIE)
	{
		aniId = ID_ANI_GOOMBA_DIE;
	}
	else if (state == GOOMBA_STATE_DIE_REVERSE)
	{
		aniId = ID_ANI_GOOMBA_DIE_REVERSE;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y, 1.0f ,&currentAniId); //&currentAniId to get current sprite id 
	//RenderBoundingBox();
}

void CGoomba::Reload()
{
	// Reset the Goomba's state and position
	CGameObject::Reload();
	SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE:
		die_start = GetTickCount64();
		y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2;
		vx = 0;
		vy = 0;
		ay = 0;
		break;

	case GOOMBA_STATE_WALKING:
		vx = -GOOMBA_WALKING_SPEED;
		ax = 0;
		ay = GOOMBA_GRAVITY;
		break;

	case GOOMBA_STATE_RISE:
		originalY = y;
		ay = 0;
		vx = 0;
		vy = -GOOMBA_RISE_SPEED;
		break;

	case GOOMBA_STATE_DIE_REVERSE:
		DebugOut(L"GOOMBA DIE REVERSE\n");
		die_start = GetTickCount64();
		vx = 0;
		vy = -GOOMBA_DEFLECT_SPEED;
		ay = GOOMBA_GRAVITY;
		break;
	}
}