#include "PiranhaPlant.h"
#include "PlayScene.h"

CPiranhaPlant::CPiranhaPlant(float x, float y) : CEnemy(x, y)
{
	this->x = x;
	this->y = y;
	originalY = y;
	SetState(PIRANHA_STATE_HIDE);
	isShooting = false;				// Initialize shooting state
}

CMario* CPiranhaPlant::GetPlayer()
{
	CGame* game = CGame::GetInstance();
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());
	return mario;
}

void CPiranhaPlant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - PIRANHA_BBOX_WIDTH / 2;
	top = y - (PIRANHA_BBOX_HEIGHT - 10.0f) / 2;
	right = left + PIRANHA_BBOX_WIDTH;
	bottom = top + PIRANHA_BBOX_HEIGHT;
}

void CPiranhaPlant::SetState(int state)
{
	stateStartTime = GetTickCount64();
	this->state = state;

	switch (state) {
	case PIRANHA_STATE_HIDE:
		isShooting = false; // Reset shooting state
		vy = 0;
		break;
	case PIRANHA_STATE_RISE:
		isShooting = false; // Reset shooting state
		vy = -PIRANHA_MOVE_SPEED;
		break;
	case PIRANHA_STATE_SNIP:
		vy = 0;
		break;
	case PIRANHA_STATE_DIVE:
		isShooting = false; // Reset shooting state
		vy = PIRANHA_MOVE_SPEED;
		break;
	default:
		break;
	}
}

int CPiranhaPlant::GetSnippingDirection()
{
	// Get the current scene and player
	CMario* mario = GetPlayer();

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	// Check the position of the Piranha Plant relative to Mario
	if (marioX < x) {
		if (marioY < y)
			return 0;		// UP_LEFT
		else
			return 1;		// DOWN_LEFT
	}
	else {
		if (marioY < y)
			return 2;		// UP_RIGHT
		else
			return 3;		// DOWN_RIGHT
	}
}

void CPiranhaPlant::Render()
{
	if (state == PIRANHA_STATE_HIDE)
		return;

	int aniId = -1;

	if (state == PIRANHA_STATE_RISE || state == PIRANHA_STATE_DIVE) {
		int direction = GetSnippingDirection();
		if (direction == 0 || direction == 1)
			aniId = PIRANHA_ANI_LEFT_RISE_DIVE;
		else
			aniId = PIRANHA_ANI_RIGHT_RISE_DIVE;
	}
	else if (state == PIRANHA_STATE_SNIP) {
		int direction = GetSnippingDirection();
		if (direction == 0)
			aniId = PIRANHA_ANI_UP_LEFT;
		else if (direction == 1)
			aniId = PIRANHA_ANI_DOWN_LEFT;
		else if (direction == 2)
			aniId = PIRANHA_ANI_UP_RIGHT;
		else
			aniId = PIRANHA_ANI_DOWN_RIGHT;
	}

	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	RenderBoundingBox();
}

void CPiranhaPlant::Shoot(int direction)
{
	CFireBullet* fireBullet = new CFireBullet(x, y);
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	currentScene->Add(fireBullet);

	CMario* mario = GetPlayer();

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);
	DebugOut(L"marioX: %f\n", marioX);
	DebugOut(L"Piranha x: %f\n", x);

	float relativeX = fabs(marioX - x);
	
	switch (direction) {
	case 0:
		if (relativeX > LIMIT_RANGE_TO_SHOOT)
			fireBullet->SetState(FIRE_BULLET_STATE_LEFT_SHOOT_UP_FAR);
		else
			fireBullet->SetState(FIRE_BULLET_STATE_LEFT_SHOOT_UP_NEAR);
		break;
	case 1:
		if (relativeX > LIMIT_RANGE_TO_SHOOT)
			fireBullet->SetState(FIRE_BULLET_STATE_LEFT_SHOOT_DOWN_FAR);
		else
			fireBullet->SetState(FIRE_BULLET_STATE_LEFT_SHOOT_DOWN_NEAR);
		break;
	case 2:
		if (relativeX > LIMIT_RANGE_TO_SHOOT)
			fireBullet->SetState(FIRE_BULLET_STATE_RIGHT_SHOOT_UP_FAR);
		else
			fireBullet->SetState(FIRE_BULLET_STATE_RIGHT_SHOOT_UP_NEAR);
		break;
	case 3:
		if (relativeX > LIMIT_RANGE_TO_SHOOT)
			fireBullet->SetState(FIRE_BULLET_STATE_RIGHT_SHOOT_DOWN_FAR);
		else
			fireBullet->SetState(FIRE_BULLET_STATE_RIGHT_SHOOT_DOWN_NEAR);
		break;
	default:
		break;
	}

	isShooting = true; // Set shooting state to true
}

bool CPiranhaPlant::IsTargetInRange() {
	CMario* mario = GetPlayer();

	float backBufferWidth = CGame::GetInstance()->GetBackBufferWidth();
	float backBufferHeight = CGame::GetInstance()->GetBackBufferHeight();
	
	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	float relativeX = fabs(marioX - x);
	float relativeY = fabs(marioY - y);

	return (relativeX < (backBufferWidth + 20.0f) / 2.0f
		&& relativeY < (backBufferHeight + 20.0f) / 2.0f);
}

void CPiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	ULONGLONG now = GetTickCount64();

	bool isInRange = IsTargetInRange();

	CMario* mario = GetPlayer();

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	float relativeX = fabs(marioX - x);
	if (relativeX < DISTANCE_PIRANHA_NOT_INVOKE) {
		if (state == PIRANHA_STATE_HIDE)
			SetState(PIRANHA_STATE_HIDE);
	}

	// Mario is in the range of snipping
	switch (state) {
	case PIRANHA_STATE_HIDE:
		if (now - stateStartTime > PIRANHA_HIDE_TIMEOUT) {
			SetState(PIRANHA_STATE_RISE);
		}
		break;

	case PIRANHA_STATE_RISE:
		y += vy * dt;
		if (fabs(y - originalY) >= PIRANHA_BBOX_HEIGHT_RISE) {
			y = originalY - PIRANHA_BBOX_HEIGHT_RISE;
			SetState(PIRANHA_STATE_SNIP);
		}
		break;

	case PIRANHA_STATE_SNIP:
		if (!isShooting && now - stateStartTime > PIRANHA_WAIT_FOR_SHOOT_TIMEOUT && IsTargetInRange()) {
			int direction = GetSnippingDirection();
			Shoot(direction);
		}

		if (now - stateStartTime > PIRANHA_SNIP_TIMEOUT) {		
			SetState(PIRANHA_STATE_DIVE);
		}
		break;

	case PIRANHA_STATE_DIVE:
		y += vy * dt;
		if (fabs(y - originalY) >= PIRANHA_BBOX_HEIGHT_RISE) {
			y = originalY + PIRANHA_BBOX_HEIGHT_RISE;
			SetState(PIRANHA_STATE_HIDE);
		}
		break;

	default:
		break;
	}

	//DebugOut(L"Piranha state: %d\n", state);
	

	CCollision::GetInstance()->Process(this, dt, coObjects);
}