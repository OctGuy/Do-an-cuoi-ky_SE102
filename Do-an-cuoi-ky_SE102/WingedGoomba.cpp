#include "WingedGoomba.h"
#include "GameObject.h"
#include "Game.h"
#include "PlayScene.h"

CWingedGoomba::CWingedGoomba(float x, float y) : CGameObject (x, y) {
	this->ax = 0;
	this->ay = GOOMBA_WING_GRAVITY;
	die_start = -1;
	tracking_start = -1;
	isOnPlatform = false;
	isBouncing = false;
	isInAir = false;
	isTrackingMario = false;
	bounceCount = 0;
	SetState(GOOMBA_WING_STATE_BOUNCE);
}

CMario* CWingedGoomba::GetPlayer() {
	CGame* game = CGame::GetInstance();
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(playScene->GetPlayer());

	return mario;
}

void CWingedGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	if (state == GOOMBA_WING_STATE_DIE) {
		left = x - GOOMBA_BASE_BBOX_WIDTH / 2;
		top = y - GOOMBA_BBOX_HEIGHT_DIE / 2;
		right = left + GOOMBA_BASE_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
	}
	else if (state == GOOMBA_WING_STATE_BOUNCE 
		|| state == GOOMBA_WING_STATE_FLY 
		|| state == GOOMBA_WING_STATE_TRACKING_MARIO) {
		left = x - GOOMBA_WING_BBOX_WIDTH / 2;
		top = y - GOOMBA_WING_BBOX_HEIGHT / 2;
		right = left + GOOMBA_WING_BBOX_WIDTH;
		bottom = top + GOOMBA_WING_BBOX_HEIGHT;
	}
	else {
		left = x - GOOMBA_BASE_BBOX_WIDTH / 2;
		top = y - 1.0f - GOOMBA_BASE_BBOX_HEIGHT / 2;
		right = left + GOOMBA_BASE_BBOX_WIDTH;
		bottom = top + GOOMBA_BASE_BBOX_HEIGHT;
	}
}

void CWingedGoomba::OnNoCollision(DWORD dt) {
	x += vx * dt;
	y += vy * dt;
	isOnPlatform = false;
}

void CWingedGoomba::OnCollisionWith(LPCOLLISIONEVENT e) {
	if (e->obj->IsBlocking()) {
		if (e->ny != 0) {
			if (isBouncing) {
				if (e->ny < 0) {
					vy = -GOOMBA_WING_BOUNCE_SPEED;
					bounceCount++;
					isOnPlatform = true;
				}
				else vy = 0;
			}
			else if (isInAir) {
				if (e->ny < 0) {
					vy = 0;
					isInAir = false;
					isOnPlatform = true;
					SetState(GOOMBA_WING_STATE_TRACKING_MARIO);
				}
				else vy = 0;
			}
		}
		else if (e->nx != 0) {
			if (e->nx > 0)
				vx = GOOMBA_WING_WALKING_SPEED;
			else
				vx = -GOOMBA_WING_WALKING_SPEED;
		}
	}

	if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
}

void CWingedGoomba::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {
	CMario* mario = GetPlayer();
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
	CKoopa* koopaHeldByMario = dynamic_cast<CKoopa*>(mario->GetKoopa());

	if (koopa) {
		if (koopaHeldByMario != nullptr && koopaHeldByMario == koopa && koopa->GetIsHeld()) {
			DebugOut(L"Koopa is collided with Goomba when Mario hold\n");
			SetState(GOOMBA_WING_STATE_DIE_REVERSE);
			koopa->SetState(KOOPA_STATE_DIE);
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE) {
			DebugOut(L"Koopa is collided with Goomba when Mario kick\n");
			SetState(GOOMBA_STATE_DIE_REVERSE);
		}

		mario->AddPoint(100, e);
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
	else if (state == GOOMBA_WING_STATE_TRACKING_MARIO)
		aniId = GOOMBA_WING_ANI_TRACKING_MARIO;

	if (aniId != -1)
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	RenderBoundingBox();
}

void CWingedGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	vx += ax * dt;
	vy += ay * dt;

	if (isBouncing && bounceCount >= 5) {
		isBouncing = false;
		SetState(GOOMBA_WING_STATE_FLY);
		bounceCount = 0;
	}

	if (isTrackingMario) {
		if (GetTickCount64() - tracking_start > GOOMBA_TRACKING_TIMEOUT) {
			SetState(GOOMBA_WING_STATE_BOUNCE);
			isTrackingMario = false;
		}
	}

	if ((state == GOOMBA_WING_STATE_DIE && GetTickCount64() - die_start > GOOMBA_DIE_TIMEOUT)
		|| (state == GOOMBA_WING_STATE_DIE_REVERSE && GetTickCount64() - die_start > GOOMBA_DIE_REVERSE_TIMEOUT)) {
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CWingedGoomba::TrackingMario() {
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mX, mY;
	player->GetPosition(mX, mY);
	vx = (x < mX) ? GOOMBA_WING_WALKING_SPEED : -GOOMBA_WING_WALKING_SPEED;
	nx = (x < mX) ? 1 : -1;
}

void CWingedGoomba::SetState(int state) {
	CGameObject::SetState(state);

	switch (state) {
	case GOOMBA_WING_STATE_FLY:
	{
		TrackingMario();
		vx = (nx > 0) ? GOOMBA_WING_WALKING_SPEED : -GOOMBA_WING_WALKING_SPEED;
		ay = GOOMBA_WING_GRAVITY;
		vy = -GOOMBA_WING_FLY_UP_SPEED;
		isTrackingMario = false;
		isBouncing = false;
		isInAir = true;
		break;
	}
		
	case GOOMBA_WING_STATE_WALKING:
	{
		isBouncing = false;
		isInAir = false;
		isTrackingMario = false;
		vx = (nx > 0) ? GOOMBA_WING_WALKING_SPEED : -GOOMBA_WING_WALKING_SPEED;
		ay = GOOMBA_WING_GRAVITY;
		break;
	}
		
	case GOOMBA_WING_STATE_BOUNCE:
	{
		vx = (nx > 0) ? GOOMBA_WING_WALKING_SPEED : -GOOMBA_WING_WALKING_SPEED;
		//TrackingMario();
		ay = GOOMBA_WING_GRAVITY;
		vy = -GOOMBA_WING_BOUNCE_SPEED;
		isBouncing = true;
		isTrackingMario = false;
		bounceCount++;
		break;
	}
		
	case GOOMBA_WING_STATE_DIE: 
	{
		DebugOut(L"GOOMBA WING DIE\n");
		die_start = GetTickCount64();
		y += (GOOMBA_BASE_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2;
		vx = 0;
		vy = 0;
		ay = 0;
		break;
	}
	case GOOMBA_WING_STATE_DIE_REVERSE:
	{
		DebugOut(L"GOOMBA WING DIE REVERSE\n");
		die_start = GetTickCount64();
		vx = 0;
		vy = -GOOMBA_WING_DEFLECT_SPEED;
		ay = GOOMBA_WING_GRAVITY;
		break;
	}
	case GOOMBA_WING_STATE_TRACKING_MARIO:
	{
		isTrackingMario = true;
		isInAir = false;
		isBouncing = false;
		tracking_start = GetTickCount64();
		TrackingMario();
		vy = 0;
		ay = GOOMBA_WING_GRAVITY;
		break;
	}
	default:
		break;
	}
}