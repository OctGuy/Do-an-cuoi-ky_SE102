#pragma once
#include "GameObject.h"

#include "Animation.h"
#include "Animations.h"

#include "debug.h"

#define MARIO_INITIAL_SPEED		0.04f

#define MARIO_WALKING_SPEED		0.09f
#define MARIO_RUNNING_SPEED		0.20f

#define MARIO_ACCEL_WALK_X	0.0001f
#define MARIO_ACCEL_RUN_X	0.0001f

#define MARIO_FRICTION		0.0002f

#define MARIO_JUMP_SPEED_Y		0.2f
#define MARIO_JUMP_RUN_SPEED_Y	0.3f
#define MARIO_MAX_JUMP_HEIGHT	70.f

#define MARIO_GRAVITY			0.0015f
#define MARIO_MAX_FALL_SPEED	0.3f
#define MARIO_SLOW_FALL_SPEED	0.05f
#define MARIO_SLOW_FALL_DURATION 500 

#define MARIO_FLYING_SPEED	-0.1f

#define MARIO_JUMP_DEFLECT_SPEED  0.2f

#pragma region MARIO_STATE
#define MARIO_STATE_DIE				-10
#define MARIO_STATE_IDLE			0
#define MARIO_STATE_WALKING_RIGHT	100
#define MARIO_STATE_WALKING_LEFT	200

#define MARIO_STATE_JUMP			300
#define MARIO_STATE_RELEASE_JUMP    301
#define MARIO_STATE_SLOW_FALL		302
#define MARIO_STATE_FLYING			303	

#define MARIO_STATE_RUNNING_RIGHT	400
#define MARIO_STATE_RUNNING_LEFT	500

#define MARIO_STATE_SIT				600
#define MARIO_STATE_SIT_RELEASE		601

#define MARIO_STATE_DECELERATE_RIGHT 700
#define MARIO_STATE_DECELERATE_LEFT  701
#pragma endregion


#pragma region ANIMATION_ID

#define ID_ANI_MARIO_CHANGE_LEVEL_RIGHT 300
#define ID_ANI_MARIO_CHANGE_LEVEL_LEFT 301

#define ID_ANI_MARIO_IDLE_RIGHT 400
#define ID_ANI_MARIO_IDLE_LEFT 401

#define ID_ANI_MARIO_WALKING_RIGHT 500
#define ID_ANI_MARIO_WALKING_LEFT 501

#define ID_ANI_MARIO_RUNNING_RIGHT 600
#define ID_ANI_MARIO_RUNNING_LEFT 601

#define ID_ANI_MARIO_JUMP_WALK_RIGHT 700
#define ID_ANI_MARIO_JUMP_WALK_LEFT 701
#define ID_ANI_MARIO_FALLING_RIGHT 702
#define ID_ANI_MARIO_FALLING_LEFT 703

#define ID_ANI_MARIO_JUMP_RUN_RIGHT 800
#define ID_ANI_MARIO_JUMP_RUN_LEFT 801

#define ID_ANI_MARIO_SIT_RIGHT 900
#define ID_ANI_MARIO_SIT_LEFT 901

#define ID_ANI_MARIO_BRACE_RIGHT 1000
#define ID_ANI_MARIO_BRACE_LEFT 1001

#define ID_ANI_MARIO_DIE 999

// SMALL MARIO
#define ID_ANI_MARIO_SMALL_IDLE_RIGHT 1100
#define ID_ANI_MARIO_SMALL_IDLE_LEFT 1102

#define ID_ANI_MARIO_SMALL_WALKING_RIGHT 1200
#define ID_ANI_MARIO_SMALL_WALKING_LEFT 1201

#define ID_ANI_MARIO_SMALL_RUNNING_RIGHT 1300
#define ID_ANI_MARIO_SMALL_RUNNING_LEFT 1301

#define ID_ANI_MARIO_SMALL_BRACE_RIGHT 1400
#define ID_ANI_MARIO_SMALL_BRACE_LEFT 1401

#define ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT 1500
#define ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT 1501

#define ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT 1600
#define ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT 1601

//RACCOON MARIO
#define ID_ANI_MARIO_RACCOON_IDLE_RIGHT 2000
#define ID_ANI_MARIO_RACCOON_IDLE_LEFT 2001

#define ID_ANI_MARIO_RACCOON_WALKING_RIGHT 2100
#define ID_ANI_MARIO_RACCOON_WALKING_LEFT 2101

#define ID_ANI_MARIO_RACCOON_RUNNING_RIGHT 2200
#define ID_ANI_MARIO_RACCOON_RUNNING_LEFT 2201

#define ID_ANI_MARIO_RACCOON_JUMP_WALK_RIGHT 2300
#define ID_ANI_MARIO_RACCOON_JUMP_WALK_LEFT 2301
#define ID_ANI_MARIO_RACCOON_FALLING_RIGHT 2302
#define ID_ANI_MARIO_RACCOON_FALLING_LEFT 2303

#define ID_ANI_MARIO_RACCOON_JUMP_RUN_RIGHT 2400
#define ID_ANI_MARIO_RACCOON_JUMP_RUN_LEFT 2401

#define ID_ANI_MARIO_RACCOON_SIT_RIGHT 2500
#define ID_ANI_MARIO_RACCOON_SIT_LEFT 2501

#define ID_ANI_MARIO_RACCOON_BRACE_RIGHT 2600
#define ID_ANI_MARIO_RACCOON_BRACE_LEFT 2601

#define ID_ANI_MARIO_RACCOON_SLOWFALL_RIGHT 2700
#define ID_ANI_MARIO_RACCOON_SLOWFALL_LEFT 2701

#define ID_ANI_MARIO_RACCOON_FLYING_RIGHT 2800
#define ID_ANI_MARIO_RACCOON_FLYING_LEFT 2801

#pragma endregion

#define GROUND_Y 350.0f


#define	MARIO_LEVEL_SMALL	1
#define	MARIO_LEVEL_BIG		2
#define	MARIO_LEVEL_RACCOON 3

#define MARIO_BIG_BBOX_WIDTH  14
#define MARIO_BIG_BBOX_HEIGHT 24
#define MARIO_BIG_SITTING_BBOX_WIDTH  14
#define MARIO_BIG_SITTING_BBOX_HEIGHT 16

#define MARIO_SIT_HEIGHT_ADJUST ((MARIO_BIG_BBOX_HEIGHT-MARIO_BIG_SITTING_BBOX_HEIGHT)/2)

#define MARIO_SMALL_BBOX_WIDTH  13
#define MARIO_SMALL_BBOX_HEIGHT 12


#define MARIO_UNTOUCHABLE_TIME 100

class CMario : public CGameObject
{
	BOOLEAN isSitting;
	BOOLEAN isRunning;

	float maxVx;
	float maxVy; 

	float ax;				// acceleration on x 
	float ay;				// acceleration on y 

	int level;
	int untouchable;
	ULONGLONG untouchable_start;
	ULONGLONG slowfall_start; 

	BOOLEAN isOnPlatform;
	BOOLEAN isInAir;	//If Raccoon mario is flying or floating this should be true
	float currentFloorY; // Y position of the current floor

	//Tracking point and coin
	int coin;
	int point;

	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithCoin(LPCOLLISIONEVENT e);
	void OnCollisionWithPortal(LPCOLLISIONEVENT e);
	void OnCollisionWithBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e);
	void OnCollisionWithPowerUp(LPCOLLISIONEVENT e);
	void OnCollisionWithBullet(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);

	int GetAniIdBig();
	int GetAniIdSmall();
	int GetAniIdRaccoon();

public:
	CMario(float x, float y) : CGameObject(x, y)
	{
		isSitting = false;
		isRunning = false;
		maxVx = MARIO_WALKING_SPEED;
		maxVy = MARIO_MAX_FALL_SPEED;
		ax = 0.0f;
		ay = MARIO_GRAVITY;

		level = MARIO_LEVEL_SMALL;
		untouchable = 0;
		untouchable_start = -1;

		isOnPlatform = false;
		isInAir = false;
		currentFloorY = GROUND_Y; // Initialize to ground level

		coin = 0;
		point = 0;
	}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void SetState(int state);

	float GetVx() { return vx; }
	float GetVy() { return vy; }
	float GetAx() { return ax; }
	int GetCoin() { return coin; }
	int GetPoint() { return point; }
	int GetLevel() { return level; }

	int IsCollidable()
	{
		return (state != MARIO_STATE_DIE);
	}

	int IsBlocking() { return (state != MARIO_STATE_DIE && untouchable == 0); }

	BOOLEAN IsOnPlatform() { return isOnPlatform; }
	BOOLEAN IsInAir() { return isInAir; }

	//Update coin and point
	void AddCoin() {coin++; AddPoint(100);}
	void AddPoint(int p) { point += p; }


	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);

	void SetLevel(int l);
	//int GetLevel() { return level; }
	void GetHurt();
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};