#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "WingedGoomba.h"
#include "Coin.h"
#include "Portal.h"
#include "QuestionBrick.h"
#include "ShinyBrick.h"
#include "PiranhaPlant.h"
#include "Koopa.h"
#include "PSwitch.h"

#include "Collision.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	//DebugOut(L"[INFO] Mario Update: %f %f\n", x, y);
	vy += ay * dt;
	vx += ax * dt;

	//DebugOut(L"[INFO] Mario Update: %f %f\n", vx, vy);

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

	//Update preLevel before current level
	preLevel = level;

	ULONGLONG now = GetTickCount64();

	// reset untouchable timer if untouchable time has passed
	if ((now - slowfall_start > MARIO_SLOW_FALL_DURATION) || vy == 0)
	{
		//DebugOut(L"Time Out\n");
		slowfall_start = 0;
		maxVy = MARIO_MAX_FALL_SPEED; // Reset max fall speed to default
		isInAir = false; // Reset isInAir to false
	}

	// reset untouchable timer if untouchable time has passed
	// In the Update() method, modify the untouchable timer check:
	if (untouchable && now - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	//Speacial animation timing (i want to make it so that the animation is not interrupted but this is the easiest method i can think of)
	if (now - tailAttack_start > MARIO_TAIL_ATTACK_TIME)
	{
		tailAttack_start = 0;
		isTailAttacking = false;
	}

	if (now - kick_start > MARIO_KICK_TIME)
	{
		kick_start = 0;
		isKicking = false;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);

	//Make sure mario doesnt go out of boundary
	if (x < 8.f) x = 8.f;
	if (y < 8.f) y = 8.f;

	float rightBoundary;
	float bottomBoundary;
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	playScene->GetBoundary(rightBoundary, bottomBoundary);

	if (x > rightBoundary - MARIO_BIG_BBOX_WIDTH - 8.f) x = rightBoundary - MARIO_BIG_BBOX_WIDTH - 8.f;

	//Handle Koopa Picking and Kicking
	if (Koopa)
	{
		if (level != MARIO_LEVEL_RACCOON)
			Koopa->SetPosition(x + nx * MARIO_BIG_BBOX_WIDTH / 2 + nx * 2.f, y - 3.f);
		else
			Koopa->SetPosition(x + nx * MARIO_BIG_BBOX_WIDTH / 2 + nx * 7.f, y - 3.f);
		Koopa->SetSpeed(0, 0);
		//If koopa is out of shell while mario is still holding it, mario is hurt
		if (Koopa->GetState() == KOOPA_STATE_WALKING_LEFT ||
			Koopa->GetState() == KOOPA_STATE_WALKING_RIGHT)
		{
			GetHurt();
			if (nx == 1)
				Koopa->SetState(KOOPA_STATE_WALKING_RIGHT);
			else
				Koopa->SetState(KOOPA_STATE_WALKING_LEFT);

			dynamic_cast<CKoopa*>(Koopa)->SetIsHeld(false);
			Koopa = NULL;
		}
		else
		{
			if (!isAbleToHold)
			{
				isKicking = true;
				kick_start = now;
				if (Koopa->GetState() == KOOPA_STATE_SHELL_IDLE ||
					Koopa->GetState() == KOOPA_STATE_SHELL_SHAKING)
					Koopa->SetState(KOOPA_STATE_SHELL_MOVE);
				else
					Koopa->SetState(KOOPA_STATE_SHELL_REVERSE_MOVE);
				Koopa->SetSpeed(nx * KOOPA_SHELL_SPEED, 0);

				dynamic_cast<CKoopa*>(Koopa)->SetIsHeld(false);
				Koopa = NULL;
			}
		}
	}
}

void CMario::AddPoint(int p, LPCOLLISIONEVENT e)
{
	point += p;
	//Get the position of the object that mario touched to add particle
	if (e != NULL)
	{
		CGame* game = CGame::GetInstance();
		CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
		float objX, objY;
		e->obj->GetPosition(objX, objY);
		CParticle* particle = new CParticle(objX, objY, PARTICLE_TYPE_POINT);
		playScene->Add(particle);
	}
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
			isInAir = false; // Reset isInAir to false because mario hit his head (OUCH!)
			slowfall_start = 0;
			float fill1, fill2, fill3; //I dont know how to get the bounding box of the object without these variable
			e->obj->GetBoundingBox(fill1, currentFloorY, fill2, fill3);
		}
	}
	else if (e->nx != 0 && e->obj->IsBlocking())
	{
		vx = 0;
	}

	if (dynamic_cast<CShinyBrick*>(e->obj))
	{
		OnCollisionWithShinyBrick(e);
	}
	else if (dynamic_cast<CQuestionBrick*>(e->obj))
	{
		OnCollisionWithQuestionBrick(e);
	}
	else if (dynamic_cast<CGoomba*>(e->obj))
	{
		OnCollisionWithGoomba(e);
	}
	else if (dynamic_cast<CCoin*>(e->obj))
	{
		OnCollisionWithCoin(e);
	}
	else if (dynamic_cast<CPortal*>(e->obj))
	{
		OnCollisionWithPortal(e);
	}
	else if (dynamic_cast<CPiranhaPlant*>(e->obj))
	{
		OnCollisionWithPiranhaPlant(e);
	}
	else if (dynamic_cast<CPowerUp*>(e->obj))
	{
		OnCollisionWithPowerUp(e);
	}
	else if (dynamic_cast<CFireBullet*>(e->obj))
	{
		OnCollisionWithBullet(e);
	}
	else if (dynamic_cast<CKoopa*>(e->obj))
	{
		OnCollisionWithKoopa(e);
	}
	else if (dynamic_cast<CBrick*>(e->obj))
	{
		OnCollisionWithBrick(e);
	}
	else if (dynamic_cast<CPSwitch*>(e->obj))
	{
		OnCollisionWithPSwitch(e);
	}
	else if (dynamic_cast<CWingedGoomba*>(e->obj))
	{
		OnCollisionWithWingedGoomba(e);
	}
}

void CMario::OnCollisionWithBrick(LPCOLLISIONEVENT e)
{
	CBrick* brick = dynamic_cast<CBrick*>(e->obj);
	if (brick->GetType() == BRICK_TYPE_DEATH)
	{
		DebugOut(L"[INFO] Mario die by brick\n");
		SetState(MARIO_STATE_DIE);
	}
}

void CMario::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e)
{
	CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);
	if (e->ny > 0 && questionBrick)
		questionBrick->Activate();
}

void CMario::OnCollisionWithShinyBrick(LPCOLLISIONEVENT e)
{
	CShinyBrick* shinyBrick = dynamic_cast<CShinyBrick*>(e->obj);
	if (e->ny > 0 && shinyBrick)
		shinyBrick->Activate();
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
	AddPoint(1000, e);

}

void CMario::OnCollisionWithPSwitch(LPCOLLISIONEVENT e)
{
	CPSwitch* pSwitch = dynamic_cast<CPSwitch*>(e->obj);
	if (e->ny < 0 && pSwitch)
	{
		pSwitch->Activate();
	}
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE && goomba->GetState() != GOOMBA_STATE_DIE_REVERSE)
		{
			goomba->SetState(GOOMBA_STATE_DIE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			AddPoint(100, e);
		}
	}
	else if (goomba->GetState() != GOOMBA_STATE_DIE && goomba->GetState() != GOOMBA_STATE_DIE_REVERSE)
	{
		GetHurt();
	}
}

void CMario::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e) {
	CWingedGoomba* wingedGoomba = dynamic_cast<CWingedGoomba*>(e->obj);

	if (e->ny < 0) {
		int state = wingedGoomba->GetState();
		if (state != GOOMBA_WING_STATE_DIE && state != GOOMBA_WING_STATE_DIE_REVERSE) {
			if (state == GOOMBA_WING_STATE_TRACKING_MARIO
				|| state == GOOMBA_WING_STATE_BOUNCE
				|| state == GOOMBA_WING_STATE_FLY) {
				DebugOut(L"[INFO] Mario jump on Winged Goomba\n");
				wingedGoomba->SetState(GOOMBA_WING_STATE_WALKING);
			}
			else if (state == GOOMBA_WING_STATE_WALKING) {
				DebugOut(L"[INFO] Mario jump on Winged Goomba (wing-lose)\n");
				wingedGoomba->SetState(GOOMBA_WING_STATE_DIE);
			}

			vy = -MARIO_JUMP_DEFLECT_SPEED;
			AddPoint(100, e);
		}
	}
	else if (wingedGoomba->GetState() != GOOMBA_WING_STATE_DIE 
		&& wingedGoomba->GetState() != GOOMBA_WING_STATE_DIE_REVERSE)
		GetHurt();
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	DebugOut(L">>> Mario touched coin >>> \n");
	e->obj->Delete();
	AddCoin();
	AddPoint(50, e);
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CMario::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	GetHurt();
}

void CMario::OnCollisionWithBullet(LPCOLLISIONEVENT e) {
	GetHurt();
}

void CMario::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {

	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

	if (e->ny < 0) {
		if (koopa->GetState() == KOOPA_STATE_WALKING_LEFT
			|| koopa->GetState() == KOOPA_STATE_WALKING_RIGHT
			|| koopa->GetState() == KOOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE) {
			koopa->SetState((koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE)
				? KOOPA_STATE_SHELL_REVERSE_IDLE
				: KOOPA_STATE_SHELL_IDLE);
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE
			|| koopa->GetState() == KOOPA_STATE_SHELL_SHAKING
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_IDLE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_SHAKING) {
			if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE
				|| koopa->GetState() == KOOPA_STATE_SHELL_SHAKING)
				koopa->SetState(KOOPA_STATE_SHELL_MOVE);
			else
				koopa->SetState(KOOPA_STATE_SHELL_REVERSE_MOVE);
			koopa->SetSpeed(nx * KOOPA_SHELL_SPEED, 0);
		}
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		AddPoint(100, e);
	}
	else {
		if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE
			|| koopa->GetState() == KOOPA_STATE_SHELL_SHAKING
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_IDLE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_SHAKING) {
			if (isAbleToHold) {			// Mario picks Koopa
				this->Koopa = koopa;
				koopa->SetIsHeld(true);
			}
			else { // Kick
				isKicking = true;
				kick_start = GetTickCount64();
				if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE
					|| koopa->GetState() == KOOPA_STATE_SHELL_SHAKING)
					koopa->SetState(KOOPA_STATE_SHELL_MOVE);
				else
					koopa->SetState(KOOPA_STATE_SHELL_REVERSE_MOVE);
				koopa->SetSpeed(nx * KOOPA_SHELL_SPEED, 0);
			}
		}
		else if (koopa->GetState() == KOOPA_STATE_WALKING_LEFT
			|| koopa->GetState() == KOOPA_STATE_WALKING_RIGHT
			|| koopa->GetState() == KOOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE) {
			Koopa = NULL;
			GetHurt();
		}
	}
}

//
// Get animation ID for small Mario
//
int CMario::GetAniIdSmall()
{
	int aniId = -1;
	if (isKicking)
	{
		if (nx >= 0)
			aniId = ID_ANI_MARIO_SMALL_KICK_RIGHT;
		else
			aniId = ID_ANI_MARIO_SMALL_KICK_LEFT;
	}
	else if (Koopa)
	{
		if (!isOnPlatform)
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_HOLDING_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_HOLDING_LEFT;
		else
		{
			if (vx == 0)
				if (nx >= 0)
					aniId = ID_ANI_MARIO_SMALL_IDLE_HOLDING_RIGHT;
				else
					aniId = ID_ANI_MARIO_SMALL_IDLE_HOLDING_LEFT;
			else
			{
				if (fabs(vx) == MARIO_RUNNING_SPEED)
					if (nx >= 0)
						aniId = ID_ANI_MARIO_SMALL_RUNNING_HOLDING_RIGHT;
					else
						aniId = ID_ANI_MARIO_SMALL_RUNNING_HOLDING_LEFT;
				else
					if (nx >= 0)
						aniId = ID_ANI_MARIO_SMALL_WALKING_HOLDING_RIGHT;
					else
						aniId = ID_ANI_MARIO_SMALL_WALKING_HOLDING_LEFT;
			}
		}
	}
	else if (!isOnPlatform)
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

	if (isKicking)
	{
		if (nx >= 0)
			aniId = ID_ANI_MARIO_KICK_RIGHT;
		else
			aniId = ID_ANI_MARIO_KICK_LEFT;
	}
	else if (Koopa) {
		if (!isOnPlatform)
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_WALK_HOLDING_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_WALK_HOLDING_LEFT;
		else
		{
			if (vx == 0)
				if (nx >= 0)
					aniId = ID_ANI_MARIO_IDLE_HOLDING_RIGHT;
				else
					aniId = ID_ANI_MARIO_IDLE_HOLDING_LEFT;
			else
			{
				if (fabs(vx) == MARIO_RUNNING_SPEED)
					if (nx >= 0)
						aniId = ID_ANI_MARIO_RUNNING_HOLDING_RIGHT;
					else
						aniId = ID_ANI_MARIO_RUNNING_HOLDING_LEFT;
				else
					if (nx >= 0)
						aniId = ID_ANI_MARIO_WALKING_HOLDING_RIGHT;
					else
						aniId = ID_ANI_MARIO_WALKING_HOLDING_LEFT;
			}
		}
	}
	else if (!isOnPlatform)
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
			if (vy < 0)
				if (nx >= 0)
					aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
				else
					aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
			else
				if (nx >= 0)
					aniId = ID_ANI_MARIO_FALLING_RIGHT;
				else
					aniId = ID_ANI_MARIO_FALLING_LEFT;
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
	if (isKicking)
	{
		if (nx >= 0)
			aniId = ID_ANI_MARIO_RACCOON_KICK_RIGHT;
		else
			aniId = ID_ANI_MARIO_RACCOON_KICK_LEFT;
	}
	else if (Koopa) {
		if (!isOnPlatform)
			if (nx >= 0)
				aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_HOLDING_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_HOLDING_LEFT;
		else
		{
			if (vx == 0)
				if (nx >= 0)
					aniId = ID_ANI_MARIO_RACCOON_IDLE_HOLDING_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_IDLE_HOLDING_LEFT;
			else
			{
				if (fabs(vx) == MARIO_RUNNING_SPEED)
					if (nx >= 0)
						aniId = ID_ANI_MARIO_RACCOON_RUNNING_HOLDING_RIGHT;
					else
						aniId = ID_ANI_MARIO_RACCOON_RUNNING_HOLDING_LEFT;
				else
					if (nx >= 0)
						aniId = ID_ANI_MARIO_RACCOON_WALKING_HOLDING_RIGHT;
					else
						aniId = ID_ANI_MARIO_RACCOON_WALKING_HOLDING_LEFT;
			}
		}
	}
	else if (isTailAttacking)
	{
		if (nx > 0)
			aniId = ID_ANI_MARIO_RACCOON_TAIL_ATTACK_RIGHT;
		else
			aniId = ID_ANI_MARIO_RACCOON_TAIL_ATTACK_LEFT;
	}
	else if (!isOnPlatform)
	{
		if (abs(vx) == MARIO_RUNNING_SPEED || (isInAir && vy < 0))
		{
			if (isInAir)
				if (nx >= 0)
					aniId = ID_ANI_MARIO_RACCOON_FLYING_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_FLYING_LEFT;
			else
				if (nx >= 0)
					aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_LEFT;
		}
		else
		{
			if (vy < 0)
				if (nx >= 0)
					aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_LEFT;
			else
				if (isInAir)
					if (nx >= 0)
						aniId = ID_ANI_MARIO_RACCOON_SLOWFALL_RIGHT;
					else
						aniId = ID_ANI_MARIO_RACCOON_SLOWFALL_LEFT;
				else
					if (nx >= 0)
						aniId = ID_ANI_MARIO_RACCOON_FALLING_RIGHT;
					else
						aniId = ID_ANI_MARIO_RACCOON_FALLING_LEFT;
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

	if (state == MARIO_STATE_DIE)
		aniId = ID_ANI_MARIO_DIE;
	//The only time where this condition is true is when mario change level
	else if (CGame::GetInstance()->IsTimeFrozen()) //show changing animation
	{
		if (preLevel == MARIO_LEVEL_RACCOON || level == MARIO_LEVEL_RACCOON)
			aniId = ID_ANI_MARIO_CHANGE_LEVEL_RACCOON;
		else
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_CHANGE_LEVEL_RIGHT;
			else
				aniId = ID_ANI_MARIO_CHANGE_LEVEL_LEFT;
		}
	}
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
		if (isInAir && vy > 0)
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
		if (vx == 0)
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
		if (vx > 0)
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
			//Camera keep confusing mario for flying when he is floating 
			//So I added to make sure mario is not seen as flying
			//And it the control feel good this too so win-win
			if (vy < 0) vy = 0.f;
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

	case MARIO_STATE_TAIL_ATTACK:
		if (level == MARIO_LEVEL_RACCOON && !isTailAttacking &&
			(GetTickCount64() - tailAttack_start > MARIO_TAIL_ATTACK_TIME + 100))
		{
			DebugOut(L"[INFO] Mario tail attack\n");
			if (Tail)
			{
				Tail->SetPosition(x, y + 4.f);
				Tail->SetState(RACCOON_TAIL_STATE_ACTIVE);
			}
			tailAttack_start = GetTickCount64();
			isTailAttacking = true;
		}
		break;

	case MARIO_STATE_SIT:
		if (isRunning && isOnPlatform) break;
		if (isOnPlatform && level != MARIO_LEVEL_SMALL)
		{
			state = MARIO_STATE_IDLE;
			//vx = 0; vy = 0.0f;
			if (!isSitting)
				y += MARIO_SIT_HEIGHT_ADJUST;
			isSitting = true;
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

	case MARIO_STATE_HOLD:
		isAbleToHold = true;
		break;

	case MARIO_STATE_DROP:
		isAbleToHold = false;
		//Koopa = NULL;
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
		top = y - 2.0f - MARIO_SMALL_BBOX_HEIGHT / 2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT + 2.0f;
	}
}

void CMario::SetLevel(int l)
{
	level = l;
	if (preLevel == MARIO_LEVEL_RACCOON || level == MARIO_LEVEL_RACCOON)
		CGame::GetInstance()->FreezeGame(240); //240 is the time it take to transform to and from raccon
	else
		CGame::GetInstance()->FreezeGame(); //The amount time freeze is unmodified

	SetState(MARIO_STATE_IDLE);

	// Adjust position to avoid falling off platform
	if (this->preLevel == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
}

void CMario::GetHurt()
{
	if (untouchable == 0)
	{
		CGame::GetInstance()->FreezeGame();
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