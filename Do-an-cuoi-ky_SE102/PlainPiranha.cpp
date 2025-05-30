#include "PlainPiranha.h"
#include "PlayScene.h"

CPlainPiranha::CPlainPiranha(float x, float y) : CEnemy(x, y)
{
	originalY = y;
	SetState(PLAIN_PIRANHA_STATE_HIDE);
	die_start = -1;
}

void CPlainPiranha::Reload()
{
	CGameObject::Reload();
	SetState(PLAIN_PIRANHA_STATE_HIDE);
	die_start = -1;
}

CMario* CPlainPiranha::GetPlayer()
{
	CGame* game = CGame::GetInstance();
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());
	return mario;
}

void CPlainPiranha::GetBoundingBox(float& left, float& top, float& right, float& bottom) {
	left = x - PLAIN_PIRANHA_BBOX_WIDTH / 2;
	top = y - PLAIN_PIRANHA_BBOX_HEIGHT / 2;
	right = left + PLAIN_PIRANHA_BBOX_WIDTH;
	bottom = top + PLAIN_PIRANHA_BBOX_HEIGHT;
}

void CPlainPiranha::Render() {
	if (state == PLAIN_PIRANHA_STATE_HIDE)
		return;

	int aniId = -1;

	if (state == PLAIN_PIRANHA_STATE_DIE)
		aniId = PLAIN_PIRANHA_ANI_DIE;
	else
		aniId = PLAIN_PIRANHA_ANI;

	if (aniId != -1)
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CPlainPiranha::SetState(int state) {
	stateStartTime = GetTickCount64();

	switch (state) {
	case PLAIN_PIRANHA_STATE_HIDE:
		vy = 0;
		break;
	case PLAIN_PIRANHA_STATE_RISE:
		vy = -PLAIN_PIRANHA_MOVE_SPEED;
		break;
	case PLAIN_PIRANHA_STATE_IDLE:
		vy = 0;
		break;
	case PLAIN_PIRANHA_STATE_DIVE:
		vy = PLAIN_PIRANHA_MOVE_SPEED;
		break;
	case PLAIN_PIRANHA_STATE_DIE:
		die_start = GetTickCount64();
		vy = 0;
		break;
	default:
		break;
	}

	CGameObject::SetState(state);
}

void CPlainPiranha::OnCollisionWith(LPCOLLISIONEVENT e) {
	if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
	else if (dynamic_cast<CParaTroopa*>(e->obj))
		OnCollisionWithParaTroopa(e);
}

void CPlainPiranha::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {
	CMario* mario = GetPlayer();
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

	if (koopa) {
		if (koopa->GetIsHeld()) {
			DebugOut(L"Koopa is collided with Plain Piranha when Mario hold\n");
			SetState(PLAIN_PIRANHA_STATE_DIE);
			koopa->SetState(KOOPA_STATE_DIE);
			mario->AddPoint(100, e);
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE) {
			DebugOut(L"Koopa is collided with Plain Piranha when Mario kick\n");
			SetState(PLAIN_PIRANHA_STATE_DIE);
			mario->AddPoint(100, e);
		}
	}
}

void CPlainPiranha::OnCollisionWithParaTroopa(LPCOLLISIONEVENT e) {
	CMario* mario = GetPlayer();
	CParaTroopa* paraTroopa = dynamic_cast<CParaTroopa*>(e->obj);

	if (paraTroopa) {
		if (paraTroopa->GetIsHeld()) {
			DebugOut(L"ParaTroopa is collided with Plain Piranha when Mario hold\n");
			SetState(PLAIN_PIRANHA_STATE_DIE);
			paraTroopa->SetState(PARATROOPA_STATE_DIE);
			mario->AddPoint(100, e);
		}
		else if (paraTroopa->GetState() == PARATROOPA_STATE_SHELL_MOVE
			|| paraTroopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_MOVE) {
			DebugOut(L"ParaTroopa is collided with Plain Piranha when Mario kick\n");
			SetState(PLAIN_PIRANHA_STATE_DIE);
			mario->AddPoint(100, e);
		}
	}
}

void CPlainPiranha::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	ULONGLONG now = GetTickCount64();
	CMario* mario = GetPlayer();

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);
	float relativeX = fabs(marioX - x);

	if (relativeX < DISTANCE_PLAIN_PIRANHA_NOT_INVOKE) {
		if (state == PLAIN_PIRANHA_STATE_HIDE)
			SetState(PLAIN_PIRANHA_STATE_HIDE);
	}

	switch (state) {
	case PLAIN_PIRANHA_STATE_HIDE:
	{
		if (now - stateStartTime > PLAIN_PIRANHA_HIDE_TIMEOUT)
			SetState(PLAIN_PIRANHA_STATE_RISE);
		break;
	}

	case PLAIN_PIRANHA_STATE_RISE:
	{
		y += vy * dt;
		if (fabs(y - originalY) >= PLAIN_PIRANHA_BBOX_HEIGHT_RISE) {
			y = originalY - PLAIN_PIRANHA_BBOX_HEIGHT_RISE;
			SetState(PLAIN_PIRANHA_STATE_IDLE);
		}
		break;
	}
		

	case PLAIN_PIRANHA_STATE_IDLE:
	{
		if (now - stateStartTime > PLAIN_PIRANHA_IDLE_TIMEOUT)
			SetState(PLAIN_PIRANHA_STATE_DIVE);
		break;
	}

	case PLAIN_PIRANHA_STATE_DIVE:
	{
		y += vy * dt;
		if (fabs(y - originalY) >= PLAIN_PIRANHA_BBOX_HEIGHT_RISE) {
			y = originalY + PLAIN_PIRANHA_BBOX_HEIGHT_RISE;
			SetState(PLAIN_PIRANHA_STATE_HIDE);
		}
		break;
	}
		
	case PLAIN_PIRANHA_STATE_DIE: 
	{
		if (now - die_start > PLAIN_PIRANHA_DIE_TIMEOUT)
			this->Delete();
		break;
	}
	default:
		break;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}