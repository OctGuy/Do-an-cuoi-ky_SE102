﻿#include <algorithm>
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
#include "PlainPiranha.h"
#include "Koopa.h"
#include "ParaTroopa.h"
#include "PSwitch.h"
#include "MovingPlatform.h"
#include "Collision.h"
#include "TunnelBlock.h"
#include "GoalRoulette.h"
#include "Boomerang.h"
#include "BoomerangBrother.h"
#include "SceneSweeper.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	//DebugOut(L"[INFO] Mario Update: %f %f\n", vx, vy);

	// Cap the falling speed to MAX_FALL_SPEED
	if (vy > maxVy) vy = maxVy;

	//Calcute vx like this so it wont get change abruptly
	if (fabs(vx) > fabs(maxVx)) {
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

	//Reset normal gravity and set isOnPlatform to true to make sure mario can still jump on falling platform
	if (isOnFallingPlatform)
	{
		isOnPlatform = true;
		currentFloorY += vy * dt; // Update current floor Y position so mario jump height stays the same
	}

	if (vy < 0) // Mario is jumping then mario cant be on platform right? could cause bug but not my problem now lol
		isOnFallingPlatform = false;

	//Update preLevel before current level
	preLevel = level;

	ULONGLONG now = GetTickCount64();

	//make sure gravity isnt apply immediately if mario is floating or flying in raccoon form
	if ((now - slowfall_start > MARIO_SLOW_FALL_DURATION || vy == 0)) //might cause problem for flying
	{
		//DebugOut(L"Time Out\n");
		slowfall_start = 0;
		maxVy = MARIO_MAX_FALL_SPEED; 
		isInAir = false; 
	}

	//Check flying time
	if (flying_start && now - flying_start > MARIO_FLYING_TIME)
	{
		//DebugOut(L"Flying Time Out\n");
		flying_start = 0;
		pMeter = 0;
		isAbleToFly = false; // Reset flying ability
	}

	// reset untouchable timer if untouchable time has passed
	// In the Update() method, modify the untouchable timer check:
	if (untouchable)
	{
		if(now - untouchable_start > MARIO_UNTOUCHABLE_TIME)
			{
				untouchable_start = 0;
				untouchable = 0;
				opacity = 1.0f;
			}
		else if (((now - untouchable_start) / MARIO_LOW_OPACITY_TIME) % 2 == 1)
			opacity = 0.8f;
		else if (((now - untouchable_start) / MARIO_LOW_OPACITY_TIME) % 2 == 0)
			opacity = 0.0f;
	}

	//Check if mario is tunneling
	if (tunnel_start && now - tunnel_start > MARIO_TUNNEL_TIME)
	{
		tunnel_start = 0;
		isTunneling = false;
		ay = MARIO_GRAVITY; // Reset gravity to default because tunnnel set ay to 0
	}

	//Speacial animation timing (i want to make it so that the animation is not interrupted but this is the easiest method i can think of)
	if (now - tailAttack_start > MARIO_TAIL_ATTACK_TIME)
	{
		tailAttack_start = 0;
		isTailAttacking = false;
		Tail->SetActive(false); 
		CAnimations* animations = CAnimations::GetInstance();
		animations->Get(ID_ANI_MARIO_RACCOON_TAIL_ATTACK_RIGHT)->Reset();
		animations->Get(ID_ANI_MARIO_RACCOON_TAIL_ATTACK_LEFT)->Reset();
	}

	if (now - kick_start > MARIO_KICK_TIME)
	{
		kick_start = 0;
		isKicking = false;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);

	//DebugOut(L"[INFO] Mario is On Platform: %d\n", isOnPlatform);
	//DebugOut(L"[INFO] Mario is current FloorY: %f\n", currentFloorY);

	if (!flying_start) {
		if (isOnPlatform && fabs(vx) > MARIO_WALKING_SPEED)
		{
			pMeter += 10.f;
			//cap pmeter
			if (pMeter > MARIO_P_METER_MAX)
				pMeter = MARIO_P_METER_MAX;
		}
		else
		{
			pMeter -= 5.f;
			if (pMeter < 0)
				pMeter = 0;
		}
	}

	if (pMeter == MARIO_P_METER_MAX)
		isAbleToFly = true;
	else
		isAbleToFly = false;

	//Make sure mario doesnt go out of boundary
	float leftBoundary;
	float rightBoundary;
	float bottomBoundary;
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	playScene->GetBoundary(leftBoundary, rightBoundary, bottomBoundary);
	
	if (x < leftBoundary + 8.f) { x = leftBoundary + 8.f; vx = 0; }
	else if (x > rightBoundary - 8.f) { x = rightBoundary - 8.f; vx = 0; }
	if (y < 8.f) { y = 8.f; vy = 0; }
	else if (y > bottomBoundary) SetState(MARIO_STATE_DIE); 

	//Handle Koopa Picking and Kicking
	if (Koopa)
	{
		if (dynamic_cast<CKoopa*>(Koopa)) 
		{
			//Adjust where koopa sprite is draw to make it look prettier
			if (level != MARIO_LEVEL_RACCOON)
				Koopa->SetPosition(x + nx * MARIO_BIG_BBOX_WIDTH / 2 + nx * 2.f, y - 3.f);
			else
				Koopa->SetPosition(x + nx * MARIO_BIG_BBOX_WIDTH / 2 + nx * 5.f, y - 3.f);
			Koopa->SetSpeed(0, 0);

			//If koopa is out of shell while mario is still holding it, mario is hurt
			if (Koopa->GetState() == KOOPA_STATE_WALKING_LEFT ||
				Koopa->GetState() == KOOPA_STATE_WALKING_RIGHT)
			{
				GetHurt();
				if (nx == 1) //Koopa direction is base on Mario direction
					Koopa->SetState(KOOPA_STATE_WALKING_RIGHT);
				else
					Koopa->SetState(KOOPA_STATE_WALKING_LEFT);

				dynamic_cast<CKoopa*>(Koopa)->SetIsHeld(false);
				Koopa = nullptr;
			}
			else if(!isAbleToHold) //Mario release koopa
			{
				DebugOut(L"[INFO] Mario kick Koopa by touch\n");
				isKicking = true;
				kick_start = now;

				//If koopa is in a wall and mario is kicking it, mario kill it
				if (dynamic_cast<CKoopa*>(Koopa)->IsInWall())
					Koopa->SetState(KOOPA_STATE_DIE);
				else
				{
					if (Koopa->GetState() == KOOPA_STATE_SHELL_IDLE ||
						Koopa->GetState() == KOOPA_STATE_SHELL_SHAKING)
						Koopa->SetState(KOOPA_STATE_SHELL_MOVE);
					else
						Koopa->SetState(KOOPA_STATE_SHELL_REVERSE_MOVE);
					//Koopa is kicked in the direction mario is looking
					Koopa->SetSpeed(nx * KOOPA_SHELL_SPEED, 0);
				}

				dynamic_cast<CKoopa*>(Koopa)->SetIsHeld(false);
				Koopa = nullptr;
			}
		}

		if (dynamic_cast<CParaTroopa*>(Koopa)) {
			//Adjust where koopa sprite is draw to make it look prettier
			if (level != MARIO_LEVEL_RACCOON)
				Koopa->SetPosition(x + nx * MARIO_BIG_BBOX_WIDTH / 2 + nx * 2.f, y - 3.f);
			else
				Koopa->SetPosition(x + nx * MARIO_BIG_BBOX_WIDTH / 2 + nx * 5.f, y - 3.f);
			Koopa->SetSpeed(0, 0);

			//If koopa is out of shell while mario is still holding it, mario is hurt
			if (Koopa->GetState() == PARATROOPA_STATE_WALKING_LEFT ||
				Koopa->GetState() == PARATROOPA_STATE_WALKING_RIGHT)
			{
				GetHurt();
				if (nx == 1) //Koopa direction is base on Mario direction
					Koopa->SetState(PARATROOPA_STATE_WALKING_RIGHT);
				else
					Koopa->SetState(PARATROOPA_STATE_WALKING_LEFT);

				dynamic_cast<CParaTroopa*>(Koopa)->SetIsHeld(false);
				Koopa = nullptr;
			}
			else
			{
				if (!isAbleToHold) //Mario release koopa
				{
					isKicking = true;
					kick_start = now;

					//If koopa is in a wall and mario is kicking it, mario kill it
					if (dynamic_cast<CParaTroopa*>(Koopa)->IsInWall())
						Koopa->SetState(PARATROOPA_STATE_DIE);
					else
					{
						if (Koopa->GetState() == PARATROOPA_STATE_SHELL_IDLE ||
							Koopa->GetState() == PARATROOPA_STATE_SHELL_SHAKING)
							Koopa->SetState(PARATROOPA_STATE_SHELL_MOVE);
						else
							Koopa->SetState(PARATROOPA_STATE_SHELL_REVERSE_MOVE);
						//Koopa is kicked in the direction mario is looking
						Koopa->SetSpeed(nx * PARATROOPA_SHELL_SPEED, 0);
					}

					dynamic_cast<CParaTroopa*>(Koopa)->SetIsHeld(false);
					Koopa = nullptr;
				}
			}
		}
	}

	//Handle Raccoon Tail
	if (Tail)
	{
		Tail->SetPosition(x + 5.f*nx, y + 4.f);
	}

	//DebugOut(L"[INFO] Mario: %d %d\n", isAbleToTunnelDown, isAbleToTunnelUp);
	//DebugOut(L"[INFO] Mario Update: %f %f\n", x, y);
	//DebugOut(L"[INFO] Mario acceleration: %f %f\n", ax, ay);
	//DebugOut(L"[INFO] Mario: %d\n", isSitting);
}

void CMario::AddPoint(int p, LPCOLLISIONEVENT e)
{
	point += p;
	//Get the position of the object that mario touched to add particle
	if (e != nullptr)
	{
		CGame* game = CGame::GetInstance();
		CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
		float objX, objY;
		e->obj->GetPosition(objX, objY);
		CParticle* particle = new CParticle(objX, objY, PARTICLE_TYPE_POINT, p);
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
	else if (dynamic_cast<CPlainPiranha*>(e->obj))
	{
		OnCollisionWithPlainPiranha(e);
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
	else if (dynamic_cast<CParaTroopa*>(e->obj))
	{
		OnCollisionWithParaTroopa(e);
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
	else if (dynamic_cast<CMovingPlatform*>(e->obj)) {
		OnCollisionWithMovingPlatform(e);
	}
	else if (dynamic_cast<CTunnelBlock*>(e->obj))
	{
		OnCollisionWithTunnelBlock(e);
	}
	else if (dynamic_cast<CGoalRoulette*>(e->obj))
	{
		OnCollisionWithGoalRoulette(e);
	}
	else if (dynamic_cast<CBoomerang*>(e->obj))
	{
		OnCollisionWithGoalBoomerang(e);
	}
	else if (dynamic_cast<CBoomerangBrother*>(e->obj))
	{
		OnCollisionWithBoomerangBrother(e);
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
	if (dynamic_cast<CPowerUp*>(e->obj)->GetType() == POWER_UP_TYPE_LEAF)
	{
		if(level != MARIO_LEVEL_RACCOON)
			SetLevel(MARIO_LEVEL_RACCOON);
		AddPoint(1000, e);
	}
	else if (dynamic_cast<CPowerUp*>(e->obj)->GetType() == POWER_UP_TYPE_MUSHROOM && level == MARIO_LEVEL_SMALL)
	{
		SetLevel(MARIO_LEVEL_BIG);
		AddPoint(1000, e);
	}
	else if (dynamic_cast<CPowerUp*>(e->obj)->GetType() == POWER_UP_TYPE_1UP_MUSHROOM)
	{
		AddPoint(0, e);
		this->live++;
	}
	e->obj->Delete();
	
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
			Bounce();
			currentFloorY = y;

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
		Bounce();
		currentFloorY = y;

		int state = wingedGoomba->GetState();
		if (state != GOOMBA_WING_STATE_DIE && state != GOOMBA_WING_STATE_DIE_REVERSE) {
			if (state != GOOMBA_WING_STATE_WALKING) {
				DebugOut(L"[INFO] Mario jump on Winged Goomba\n");
				wingedGoomba->SetState(GOOMBA_WING_STATE_WALKING);
			}
			else if (state == GOOMBA_WING_STATE_WALKING) {
				DebugOut(L"[INFO] Mario jump on Winged Goomba (wing-lose)\n");
				wingedGoomba->SetState(GOOMBA_WING_STATE_DIE);
			}
			AddPoint(100, e);
		}
	}
	else if (e->nx != 0 || e->ny > 0)
	{
		if (wingedGoomba->GetState() != GOOMBA_WING_STATE_DIE
			&& wingedGoomba->GetState() != GOOMBA_WING_STATE_DIE_REVERSE)
			GetHurt();
	}
}

void CMario::OnCollisionWithMovingPlatform(LPCOLLISIONEVENT e) //Unstable need to be work on more
{
	CMovingPlatform* platform = dynamic_cast<CMovingPlatform*>(e->obj);
	if (e->ny < 0) {
		isOnFallingPlatform = true;
		vy = MOVING_PLATFORM_VERTICAL_SPEED;
		ay = MOVING_PLATFORM_GRAVITY;
	}
	else if (e->nx != 0)
	{
		float mVx, mVy;
		e->obj->GetSpeed(mVx, mVy);
		if (e->nx > 0)
		{
			vx = mVx * 1.7f; 
			ax = -MARIO_FRICTION;
		}
		else
		{
			vx = mVx * 1.7f; 
			ax = MARIO_FRICTION;
		}
	}
}

void CMario::OnCollisionWithTunnelBlock(LPCOLLISIONEVENT e)
{
	CTunnelBlock* tunnelBlock = dynamic_cast<CTunnelBlock*>(e->obj);
	if (e->ny != 0) 
	{
		if (e->ny < 0 && isAbleToTunnelDown && tunnelBlock->GetType() == TUNNEL_BLOCK_TYPE_ENTER)
		{
			DebugOut(L"Tunnel Down\n");
			SetState(MARIO_STATE_TUNNEL_DOWN);
			y += 1.f; 
		}
		else if (e->ny > 0 && isAbleToTunnelUp && tunnelBlock->GetType() == TUNNEL_BLOCK_TYPE_ENTER)
		{
			DebugOut(L"Tunnel Up\n");
			SetState(MARIO_STATE_TUNNEL_UP);
			y -= 1.f; 
		}
	}
	else if (e->nx == 0 && e->ny == 0)
	{
		if (!isTunneling && tunnelBlock->GetType() == TUNNEL_BLOCK_TYPE_ENTER 
			&& (isAbleToTunnelDown || isAbleToTunnelUp)) //Teleport to the correct destinarion after finishing tunneling
		{
			float destX, destY;
			tunnelBlock->GetDestination(destX, destY);
			SetPosition(destX, destY);
		}
		else //Doesnt really do anything, mostly for animation purpose
		{
			if (tunnelBlock->GetType() == TUNNEL_BLOCK_TYPE_UP)
			{
				SetState(MARIO_STATE_TUNNEL_UP);
			}
			else if (tunnelBlock->GetType() == TUNNEL_BLOCK_TYPE_DOWN)
			{
				SetState(MARIO_STATE_TUNNEL_DOWN);
			}
		}
	}
}

void CMario::OnCollisionWithGoalRoulette(LPCOLLISIONEVENT e)
{
	CGoalRoulette* goalRoulette = dynamic_cast<CGoalRoulette*>(e->obj);
	int card = goalRoulette->GetCurrentCard();
	CGame* game = CGame::GetInstance();
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	float objX, objY;
	e->obj->GetPosition(objX, objY);

	CParticle* particle = nullptr;

	if (card == CARD_TYPE_MUSHROOM)
		particle = new CParticle(objX, objY, PARTICLE_TYPE_MUSHROOM);
	else if (card == CARD_TYPE_PLANT)
		particle = new CParticle(objX, objY, PARTICLE_TYPE_PLANT);
	else if (card == CARD_TYPE_STAR)
		particle = new CParticle(objX, objY, PARTICLE_TYPE_STAR);

	if (particle)
		playScene->Add(particle);

	float camX, camY;
	game->GetCamPos(camX, camY);

	//Course end text
	particle = new CParticle(camX + 124.f, camY + 32.f, PARTICLE_TYPE_TEXT_1);
	if (particle)
		playScene->Add(particle);

	//You got a card text
	particle = new CParticle(camX + 108.f, camY + 48.f, PARTICLE_TYPE_TEXT_2);
	if (particle)
		playScene->Add(particle);

	//Card
	if (card == CARD_TYPE_MUSHROOM)
		particle = new CParticle(camX + 188.f, camY + 56.f, PARTICLE_TYPE_CARD_MUSHROOM);
	else if (card == CARD_TYPE_PLANT)
		particle = new CParticle(camX + 188.f, camY + 56.f, PARTICLE_TYPE_CARD_PLANT);
	else if (card == CARD_TYPE_STAR)
		particle = new CParticle(camX + 188.f, camY + 56.f, PARTICLE_TYPE_CARD_STAR);

	playScene->Add(particle);

	// Add the card to the cards vector 
	cards.push_back(card);

	// Force mrio to walk right
	SetState(MARIO_STATE_WALKING_RIGHT);
	isInputBlocked = true; //Restrict input

	//This entire part is just to let mario walk off the screen
	float rightBoundary;
	float dummy1, dummy2;
	playScene->GetBoundary(dummy1, rightBoundary, dummy2);
	playScene->SetCamRightBound(rightBoundary + 320.f);
	playScene->SetCamFollowOnX(false);

	goalRoulette->Delete();
}

void CMario::OnCollisionWithGoalBoomerang(LPCOLLISIONEVENT e)
{
	GetHurt();
}

void CMario::OnCollisionWithBoomerangBrother(LPCOLLISIONEVENT e)
{
	CBoomerangBrother* boomerangBrother = dynamic_cast<CBoomerangBrother*>(e->obj);
	DebugOut(L"[INFO] Mario collided with Boomerang Brother\n");
	if (e->ny < 0)
	{
		//DebugOut(L"[INFO] Mario jump on Boomerang Brother\n");
		if (boomerangBrother->GetState() != BOOMERANG_BROTHER_STATE_DIE)
		{
			boomerangBrother->SetState(BOOMERANG_BROTHER_STATE_DIE);
			Bounce();
			currentFloorY = y;
			AddPoint(1000, e);
		}
	}
	else if (boomerangBrother->GetState() != BOOMERANG_BROTHER_STATE_DIE)
	{
		GetHurt();
	}
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

void CMario::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e) {
	CPiranhaPlant* piranhaPlant = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (piranhaPlant) {
		if (piranhaPlant->GetState() != PIRANHA_STATE_HIDE
			&& piranhaPlant->GetState() != PIRANHA_STATE_DIE)
		GetHurt();
	}
}

void CMario::OnCollisionWithPlainPiranha(LPCOLLISIONEVENT e) {
	CPlainPiranha* plainPiranha = dynamic_cast<CPlainPiranha*>(e->obj);

	if (plainPiranha) {
		if (plainPiranha->GetState() != PLAIN_PIRANHA_STATE_HIDE
			&& plainPiranha->GetState() != PLAIN_PIRANHA_STATE_DIE)
			GetHurt();
	}
}

void CMario::OnCollisionWithBullet(LPCOLLISIONEVENT e) {
	GetHurt();
}

void CMario::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {

	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

	if (e->ny < 0) {
		Bounce();
		currentFloorY = y;

		if (koopa->GetState() == KOOPA_STATE_FLY) 
			koopa->SetState(KOOPA_STATE_WALKING_LEFT);
		else if (koopa->GetState() == KOOPA_STATE_WALKING_LEFT
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
				|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_SHAKING) 
		{
			if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE
				|| koopa->GetState() == KOOPA_STATE_SHELL_SHAKING)
				koopa->SetState(KOOPA_STATE_SHELL_MOVE);
			else
				koopa->SetState(KOOPA_STATE_SHELL_REVERSE_MOVE);

			koopa->SetSpeed(nx * KOOPA_SHELL_SPEED, 0);
		}
		AddPoint(100, e);
	}
	else if (e->nx != 0) {
		if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE
			|| koopa->GetState() == KOOPA_STATE_SHELL_SHAKING
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_IDLE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_SHAKING) {
			if (!isAbleToHold || Koopa) 	// Mario kick Koopa
			{
				DebugOut(L"[INFO] Mario kick Koopa by touch\n");
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
				|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE
				|| koopa->GetState() == KOOPA_STATE_FLY) 
		{
			Koopa = nullptr;
			GetHurt();
		}
	}
	else if (e->ny > 0) {
		if (koopa->GetState() == KOOPA_STATE_WALKING_LEFT
			|| koopa->GetState() == KOOPA_STATE_WALKING_RIGHT
			|| koopa->GetState() == KOOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE
			|| koopa->GetState() == KOOPA_STATE_FLY)
		{
			DebugOut(L"[INFO] Mario hit Koopa from above\n");
			GetHurt();
		}
	}
	else if (e->nx == 0 && e->ny == 0)
	{
		if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE
			|| koopa->GetState() == KOOPA_STATE_SHELL_SHAKING
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_IDLE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_SHAKING)
		{
			if (isAbleToHold && !Koopa) 	// Mario picks Koopa
			{
				this->Koopa = koopa;
				koopa->SetIsHeld(true);
			}
		}
	}
}

void CMario::OnCollisionWithParaTroopa(LPCOLLISIONEVENT e) {
	CParaTroopa* koopa = dynamic_cast<CParaTroopa*>(e->obj);

	if (e->ny < 0) {
		Bounce();
		currentFloorY = y;

		if (koopa->GetState() == PARATROOPA_STATE_WALKING_LEFT
			|| koopa->GetState() == PARATROOPA_STATE_WALKING_RIGHT
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_MOVE) {
			koopa->SetState((koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_MOVE)
				? PARATROOPA_STATE_SHELL_REVERSE_IDLE
				: PARATROOPA_STATE_SHELL_IDLE);
			DebugOut(L"[INFO] Mario jump on ParaTroopa\n");
		}
		else if (koopa->GetState() == PARATROOPA_STATE_BOUNCE_LEFT
			|| koopa->GetState() == PARATROOPA_STATE_BOUNCE_RIGHT) {
			DebugOut(L"[INFO] Mario jump on ParaTroopa (bounce)\n");
			koopa->SetState((koopa->GetState() == PARATROOPA_STATE_BOUNCE_LEFT)
				? PARATROOPA_STATE_WALKING_LEFT
				: PARATROOPA_STATE_WALKING_RIGHT);
		}
		else if (koopa->GetState() == PARATROOPA_STATE_SHELL_IDLE
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_SHAKING
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_IDLE
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_SHAKING)
		{
			DebugOut(L"[INFO] Mario jump on ParaTroopa (shell)\n");
			if (koopa->GetState() == PARATROOPA_STATE_SHELL_IDLE
				|| koopa->GetState() == PARATROOPA_STATE_SHELL_SHAKING)
				koopa->SetState(PARATROOPA_STATE_SHELL_MOVE);
			else
				koopa->SetState(PARATROOPA_STATE_SHELL_REVERSE_MOVE);

			koopa->SetSpeed(nx * PARATROOPA_SHELL_SPEED, 0);
		}
		AddPoint(100, e);
	}
	else if (e->nx != 0) {
		if (koopa->GetState() == PARATROOPA_STATE_SHELL_IDLE
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_SHAKING
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_IDLE
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_SHAKING) {
			if (!isAbleToHold || Koopa) 	// Mario kick Koopa
			{
				DebugOut(L"[INFO] Mario kick Koopa by touch\n");
				isKicking = true;
				kick_start = GetTickCount64();
				if (koopa->GetState() == PARATROOPA_STATE_SHELL_IDLE
					|| koopa->GetState() == PARATROOPA_STATE_SHELL_SHAKING)
					koopa->SetState(PARATROOPA_STATE_SHELL_MOVE);
				else
					koopa->SetState(PARATROOPA_STATE_SHELL_REVERSE_MOVE);
				koopa->SetSpeed(nx * PARATROOPA_SHELL_SPEED, 0);
			}
		}
		else if (koopa->GetState() == PARATROOPA_STATE_WALKING_LEFT
			|| koopa->GetState() == PARATROOPA_STATE_WALKING_RIGHT
			|| koopa->GetState() == PARATROOPA_STATE_BOUNCE_LEFT
			|| koopa->GetState() == PARATROOPA_STATE_BOUNCE_RIGHT
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_MOVE) {
			GetHurt();
			DebugOut(L"[INFO] ParaTroopa hit Mario from side\n");
			Koopa = nullptr;
		}
	}
	else if (e->ny > 0) {
		if (koopa->GetState() == PARATROOPA_STATE_WALKING_LEFT
			|| koopa->GetState() == PARATROOPA_STATE_WALKING_RIGHT
			|| koopa->GetState() == PARATROOPA_STATE_BOUNCE_LEFT
			|| koopa->GetState() == PARATROOPA_STATE_BOUNCE_RIGHT
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_MOVE) {
			DebugOut(L"[INFO] Mario hit ParaTroopa from above\n");
			GetHurt();
		}	
	}
	else if (e->nx == 0 && e->ny == 0)
	{
		if (koopa->GetState() == PARATROOPA_STATE_SHELL_IDLE
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_SHAKING
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_IDLE
			|| koopa->GetState() == PARATROOPA_STATE_SHELL_REVERSE_SHAKING)
		{
			if (isAbleToHold && !Koopa) 	// Mario picks Koopa
			{
				this->Koopa = koopa;
				koopa->SetIsHeld(true);
			}
		}
	}
}

//
// Get animation ID for small Mario
//
int CMario::GetAniIdSmall()
{
	int aniId = -1;
	if (isTunneling)
	{
		aniId = ID_ANI_MARIO_SMALL_FACE_THE_SCREEN;
	}
	else if (isKicking)
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
	if (isTunneling)
	{
		aniId = ID_ANI_MARIO_FACE_THE_SCREEN;
	}
	else if (isKicking)
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
	if (isTunneling)
	{
		aniId = ID_ANI_MARIO_RACCOON_FACE_THE_SCREEN;
	}
	else if (isKicking)
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

	animations->Get(aniId)->Render(x, y, opacity);

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
		//DebugOut(L"[INFO] Mario flying\n");
		vy = MARIO_FLYING_SPEED; // Apply upward boost
		slowfall_start = GetTickCount64();
		maxVy = MARIO_FLYING_SPEED; // Use flying speed as max speed
		isInAir = true;
		ay = 0; // Temporarily remove gravity
		break;

	case MARIO_STATE_TAIL_ATTACK:
		if (level == MARIO_LEVEL_RACCOON && !isTailAttacking &&
			(GetTickCount64() - tailAttack_start > MARIO_TAIL_ATTACK_TIME + 100))
		{
			//DebugOut(L"[INFO] Mario tail attack\n");
			if (Tail)
			{
				Tail->SetPosition(x, y + 6.f);
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
			//state = MARIO_STATE_IDLE;
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
		live--;
		SetLevel(MARIO_LEVEL_SMALL); 
		CGame::GetInstance()->Save();
		break;

	case MARIO_STATE_HOLD:
		isAbleToHold = true;
		break;

	case MARIO_STATE_DROP:
		isAbleToHold = false;
		//Koopa = NULL;
		break;

	case MARIO_STATE_TUNNEL_DOWN:
		isTunneling = true;
		tunnel_start = GetTickCount64();
		currentFloorY = y;
		isSitting = false;
		isOnPlatform = true;
		StartUntouchable();
		ay = 0;
		ax = 0;
		vx = 0;
		vy = MARIO_TUNNELING_SPEED;
		break;

	case MARIO_STATE_TUNNEL_UP:
		isTunneling = true;
		tunnel_start = GetTickCount64();
		currentFloorY = y;
		isSitting = false;
		isOnPlatform = true;
		StartUntouchable();
		ay = 0;
		ax = 0;
		vx = 0;
		vy = -MARIO_TUNNELING_SPEED;
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

void CMario::Bounce()
{
	if (isAbleToHighBounce)
	{
		ay = 0;
		currentFloorY -= MAARIO_HIGH_BOUNCE_DISTANCE;
	}
	vy = -MARIO_JUMP_DEFLECT_SPEED;
}
