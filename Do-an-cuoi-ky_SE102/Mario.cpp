#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "QuestionBrick.h"
#include "PiranhaPlant.h"

#include "Collision.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	//DebugOut(L"[INFO] Mario Update: %f %f\n", x, y);
	vy += ay * dt;
	vx += ax * dt;

	DebugOut(L"[INFO] Mario Update: %f %f\n", vx, vy);

	// Cap the falling speed to MAX_FALL_SPEED
	if (vy > maxVy) vy = maxVy;

	//Calcute vx like this so it wont get change abruptly
	if (abs(vx) > abs(maxVx)) {
		if (vx < 0 && ax > 0 || vx > 0 && ax < 0)
			vx += ax * dt;
		else
			vx = maxVx;
			//vx = maxVx;
	}

	// Compare mario y position with max jump y (calulate from the current floor - max jump height)
	// If mario y is smaller or equal to the  max jump y, set gravity to default
	if (y <= currentFloorY - MARIO_MAX_JUMP_HEIGHT)
	{
		ay = MARIO_GRAVITY;
	}

	//reset is On platform for correct jumpinga animation
	isOnPlatform = false;

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - slowfall_start > MARIO_SLOW_FALL_DURATION)
	{
		//DebugOut(L"Time Out\n");
		slowfall_start = 0;
		maxVy = MARIO_MAX_FALL_SPEED; // Reset max fall speed to default
		isInAir = false; // Reset isInAir to false
	}
	else
		//DebugOut(L"Floating\n");

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
		ay = MARIO_GRAVITY; // Reset gravity to default
		// If collosion is from below mario, mark on platform to true
		// Also update currentFloorY
		if (e->ny < 0 && abs(e->ny) > abs(e->nx))
		{
			isOnPlatform = true;
			float fill1, fill2, fill3; //I dont know how to get the bounding box of the object without these variable
			e->obj->GetBoundingBox(fill1, currentFloorY, fill2, fill3);
		}
	}
	else if (e->nx != 0 && e->obj->IsBlocking())
	{
		vx = 0;
	}

	if (dynamic_cast<CQuestionBrick*>(e->obj))
		OnCollisionWithBrick(e);
	else if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CCoin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<CPortal*>(e->obj))
		OnCollisionWithPortal(e);
	else if (dynamic_cast<CPiranhaPlant*>(e->obj))
		OnCollisionWithPiranhaPlant(e);
	else if (dynamic_cast<CPowerUp*>(e->obj))
		OnCollisionWithPowerUp(e);
}

void CMario::OnCollisionWithBrick(LPCOLLISIONEVENT e)
{
	CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);
	questionBrick->OnCollisionWith(e);
}

void CMario::OnCollisionWithPowerUp(LPCOLLISIONEVENT e)
{
	if (dynamic_cast<CPowerUp*>(e->obj)->GetType() == POWER_UP_TYPE_LEAF && level != MARIO_LEVEL_RACCOON)
	{
		SetLevel(MARIO_LEVEL_RACCOON);
	}
	else if (dynamic_cast<CPowerUp*>(e->obj)->GetType() == POWER_UP_TYPE_MUSHROOM && level == MARIO_LEVEL_SMALL)
	{
		SetLevel(MARIO_LEVEL_BIG);
	}
	e->obj->Delete();
	AddPoint(1000);
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE)
		{
			goomba->SetState(GOOMBA_STATE_DIE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			AddPoint(100);
		}
	}
	else if(goomba->GetState() != GOOMBA_STATE_DIE)
	{
		GetHurt();
	}
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	DebugOut(L">>> Mario touched coin >>> \n");
	e->obj->Delete();
	AddCoin();
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CMario::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e) 
{
	//CPiranhaPlant* piranhaPlant = dynamic_cast<CPiranhaPlant*>(e->obj);
	GetHurt();
}

//
// Get animation ID for small Mario
//
int CMario::GetAniIdSmall()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(vx) == MARIO_RUNNING_SPEED)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
		}
	}
	else if (isSitting)
	{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_SIT_LEFT;
	}
	else
	{
		if (isRunning)
		{
			if (ax > 0)
			{
				if (ax == MARIO_FRICTION * 2.f)
					aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
				else if (vx == MARIO_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
				else
					aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
			}
			else if (ax < 0)
			{
				if (ax == -MARIO_FRICTION * 2.f)
					aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
				else if (vx == -MARIO_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
				else
					aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
			}
		}
	}

	if (aniId == -1 && nx == 1) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
	else if (aniId == -1 && nx == -1) aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;

	return aniId;
}


//
// Get animdation ID for big Mario
//
int CMario::GetAniIdBig()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(vx) == MARIO_RUNNING_SPEED)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
		}
	}
	else if (isSitting)
	{
		if (nx > 0)
			aniId = ID_ANI_MARIO_SIT_RIGHT;
		else
			aniId = ID_ANI_MARIO_SIT_LEFT;
	}
	else
	{
		if (isRunning)
		{
			if (ax > 0)
			{
				if (ax == MARIO_FRICTION * 2.f)
					aniId = ID_ANI_MARIO_BRACE_LEFT;
				else if (vx == MARIO_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_RUNNING_RIGHT;
				else
					aniId = ID_ANI_MARIO_WALKING_RIGHT;
			}
			else if (ax < 0)
			{
				if (ax == -MARIO_FRICTION * 2.f)
					aniId = ID_ANI_MARIO_BRACE_RIGHT;
				else if (vx == -MARIO_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_RUNNING_LEFT;
				else
					aniId = ID_ANI_MARIO_WALKING_LEFT;
			}
		}
	}

	if (aniId == -1 && nx == 1) aniId = ID_ANI_MARIO_IDLE_RIGHT;
	else if (aniId == -1 && nx == -1) aniId = ID_ANI_MARIO_IDLE_LEFT;

	return aniId;
}

int CMario::GetAniIdRaccoon()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(vx) == MARIO_RUNNING_SPEED)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_LEFT;
		}
	}
	else if (isSitting)
	{
		if (nx > 0)
			aniId = ID_ANI_MARIO_RACCOON_SIT_RIGHT;
		else
			aniId = ID_ANI_MARIO_RACCOON_SIT_LEFT;
	}
	else
	{
		if (isRunning)
		{
			if (ax > 0)
			{
				if (ax == MARIO_FRICTION * 2.f)
					aniId = ID_ANI_MARIO_RACCOON_BRACE_LEFT;
				else if (vx == MARIO_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_RACCOON_RUNNING_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_WALKING_RIGHT;
			}
			else if (ax < 0)
			{
				if (ax == -MARIO_FRICTION * 2.f)
					aniId = ID_ANI_MARIO_RACCOON_BRACE_RIGHT;
				else if (vx == -MARIO_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_RACCOON_RUNNING_LEFT;
				else
					aniId = ID_ANI_MARIO_RACCOON_WALKING_LEFT;
			}
		}
	}

	if (aniId == -1 && nx == 1) aniId = ID_ANI_MARIO_RACCOON_IDLE_RIGHT;
	else if (aniId == -1 && nx == -1) aniId = ID_ANI_MARIO_RACCOON_IDLE_LEFT;

	return aniId;
}


void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	//The only time where this condition is true is when mario change level
	if (CGame::GetInstance()->IsTimeFrozen()) //show changing animation
	{
		if (nx > 0)
			aniId = ID_ANI_MARIO_CHANGE_LEVEL_RIGHT;
		else
			aniId = ID_ANI_MARIO_CHANGE_LEVEL_LEFT;
	}
	else if (state == MARIO_STATE_DIE)
		aniId = ID_ANI_MARIO_DIE;
	else if (level == MARIO_LEVEL_BIG)
		aniId = GetAniIdBig();
	else if (level == MARIO_LEVEL_SMALL)
		aniId = GetAniIdSmall();
	else if (level == MARIO_LEVEL_RACCOON)
		aniId = GetAniIdRaccoon();

	animations->Get(aniId)->Render(x, y);
	
	RenderBoundingBox();

	DebugOutTitle(L"Coins: %d", coin);
}

void CMario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return;

	switch (state)
	{
		case MARIO_STATE_RUNNING_LEFT:
			if(isInAir && vy > 0)
				vx = -MARIO_WALKING_SPEED;
			// Only stop sitting if on platform
			if (isSitting && isOnPlatform) {
				isSitting = false;
				y -= MARIO_SIT_HEIGHT_ADJUST;
			}
			isRunning = true;
			maxVx = -MARIO_RUNNING_SPEED;
			if (vx > 0)
				// When mario try to run left while decelerating right
				// ax to the left can be double friction right? (left momentum + friction)
				ax = -MARIO_FRICTION * 2.f;
			else
				ax = -MARIO_ACCEL_RUN_X;
			if(vx == 0)
				vx = -MARIO_INITIAL_SPEED;
			nx = -1;
			break;

		case MARIO_STATE_RUNNING_RIGHT:
			if (isInAir && vy > 0)
				vx = MARIO_WALKING_SPEED;
			// Only stop sitting if on platform
			if (isSitting && isOnPlatform) {
				isSitting = false;
				y -= MARIO_SIT_HEIGHT_ADJUST;
			}
			isRunning = true;
			maxVx = MARIO_RUNNING_SPEED;
			if (vx < 0)
				ax = MARIO_FRICTION * 2.f;
			else
				ax = MARIO_ACCEL_RUN_X;
			if (vx == 0)
				vx = MARIO_INITIAL_SPEED;
			nx = 1;
			break;

		case MARIO_STATE_WALKING_LEFT:
			// Only stop sitting if on platform
			if (isSitting && isOnPlatform) {
				isSitting = false;
				y -= MARIO_SIT_HEIGHT_ADJUST;
			}
			isRunning = true;
			maxVx = -MARIO_WALKING_SPEED;
			if(vx > 0)
				ax = -MARIO_FRICTION * 2.f; 
			else
				ax = -MARIO_ACCEL_WALK_X;
			if (vx == 0)
				vx = -MARIO_INITIAL_SPEED;
			nx = -1;
			break;

		case MARIO_STATE_WALKING_RIGHT:
			// Only stop sitting if on platform
			if (isSitting && isOnPlatform) {
				isSitting = false;
				y -= MARIO_SIT_HEIGHT_ADJUST;
			}
			isRunning = true;
			maxVx = MARIO_WALKING_SPEED;
			if (vx < 0)
				ax = MARIO_FRICTION * 2.f;
			else
				ax = MARIO_ACCEL_WALK_X;
			if (vx == 0)
				vx = MARIO_INITIAL_SPEED;
			nx = 1;
			break;

		case MARIO_STATE_JUMP:
			// Allow jumping in sitting state
			if (isOnPlatform)
			{
				ay = 0;
				if (abs(this->vx) == MARIO_RUNNING_SPEED)
					vy = -MARIO_JUMP_RUN_SPEED_Y;
				else
					vy = -MARIO_JUMP_SPEED_Y;
			}
			break;

		case MARIO_STATE_SLOW_FALL:
			// Only apply slow fall if already in the air
			if (!isOnPlatform)
			{
				DebugOut(L"[INFO] Mario floating\n");
				slowfall_start = GetTickCount64();
				maxVy = MARIO_SLOW_FALL_SPEED;
				isInAir = true;
				//ay = 0; // Temporarily remove gravity
			}
			break;

		case MARIO_STATE_RELEASE_JUMP:
			ay = MARIO_GRAVITY;
			break;

		case MARIO_STATE_FLYING:
			// Only apply flying if already in the air
			if (!isOnPlatform)
			{
				DebugOut(L"[INFO] Mario flying\n");
				vy = MARIO_FLYING_SPEED; // Apply upward boost
				slowfall_start = GetTickCount64();
				maxVy = MARIO_FLYING_SPEED; // Use flying speed as max speed
				isInAir = true;
				ay = 0; // Temporarily remove gravity
			}
			break;

		case MARIO_STATE_SIT:
			if (isRunning && isOnPlatform) break;
			if (isOnPlatform && level != MARIO_LEVEL_SMALL)
			{
				state = MARIO_STATE_IDLE;
				isSitting = true;
				//vx = 0; vy = 0.0f;
				if (!isSitting)
					y += MARIO_SIT_HEIGHT_ADJUST;
			}
			break;

		case MARIO_STATE_SIT_RELEASE:
			if (isSitting && isOnPlatform)
			{
				isSitting = false;
				state = MARIO_STATE_IDLE;
				if (!isSitting)
					y -= MARIO_SIT_HEIGHT_ADJUST;
			}
			break;

		case MARIO_STATE_DECELERATE_RIGHT:
			isRunning = false;
			ax = -MARIO_FRICTION;  // set acceleration in opposite direction
			nx = 1; //maintain right-facing animation
			break;

		case MARIO_STATE_DECELERATE_LEFT:
			isRunning = false;
			ax = MARIO_FRICTION;  // set acceleration in opposite direction
			nx = -1; //maintain left-facing animation
			break;

		case MARIO_STATE_IDLE:
			ax = 0.0f;
			vx = 0.0f;
			isRunning = false;
			break;

		case MARIO_STATE_DIE:
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			vx = 0;
			ax = 0;
			isRunning = false;
			break;
	}

	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (level == MARIO_LEVEL_BIG || level == MARIO_LEVEL_RACCOON)
	{
		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else
		{
			left = x - MARIO_BIG_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH / 2;
		top = y - MARIO_SMALL_BBOX_HEIGHT / 2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::SetLevel(int l)
{
	// Adjust position to avoid falling off platform

	CGame::GetInstance()->FreezeGame(); // time is only frozen when mario is changing level

	SetState(MARIO_STATE_IDLE); 

	if (this->level == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
	level = l;
}

void CMario::GetHurt()
{
	if (untouchable == 0)
	{
		if (level > MARIO_LEVEL_SMALL)
		{
			SetLevel(level - 1);
			StartUntouchable();
		}
		else
		{
			DebugOut(L">>> Mario DIE >>> \n");
			SetState(MARIO_STATE_DIE);
		}
	}
}

