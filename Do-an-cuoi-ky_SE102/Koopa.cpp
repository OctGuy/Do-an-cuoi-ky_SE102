#include "Koopa.h"
#include "ShinyBrick.h"
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
	if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);

	if (e->ny < 0 && e->obj->IsBlocking()) { // Stand on platform
		vy = 0;
		if (state == KOOPA_STATE_SHELL_REVERSE_JUMP)
			SetState(KOOPA_STATE_SHELL_REVERSE_IDLE);
		ay = KOOPA_GRAVITY; 
	}

	if (e->nx != 0 && e->obj->IsBlocking()) {
		if (state == KOOPA_STATE_WALKING_LEFT 
			|| state == KOOPA_STATE_WALKING_RIGHT) 
			SetState(e->nx > 0 
				? KOOPA_STATE_WALKING_RIGHT 
				: KOOPA_STATE_WALKING_LEFT);
		else if (state == KOOPA_STATE_SHELL_MOVE 
				|| state == KOOPA_STATE_SHELL_REVERSE_MOVE)
			vx = e->nx > 0 
				? KOOPA_SHELL_SPEED 
				: -KOOPA_SHELL_SPEED;
	}

	if (e->nx == 0 && e->ny == 0 && e->obj->IsBlocking())	isInWall = true;

	if (dynamic_cast<CShinyBrick*>(e->obj))
		OnCollisionWithShinyBrick(e);
	else if (dynamic_cast<CQuestionBrick*>(e->obj))
		OnCollisionWithBrick(e);
}

void CKoopa::OnCollisionWithBrick(LPCOLLISIONEVENT e) {
	CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);

	if (e->nx != 0 && state == KOOPA_STATE_SHELL_MOVE || state == KOOPA_STATE_SHELL_REVERSE_MOVE)
		questionBrick->OnCollisionWith(e);
	else if (e->ny < 0)
	{
		//DebugOut(L"[INFO] Koopa hit QuestionBrick from above\n");
		questionBrick->SetKoopa(this);
	}
}

void CKoopa::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {
	CMario* mario = GetPlayer();
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

	if (koopa) {
		// ParaTroopa collides with another ParaTroopa
		if (koopa->GetIsHeld()) {
			//DebugOut(L"[INFO] Koopa is held by Mario collide with another Koopa\n");
			koopa->SetState(KOOPA_STATE_DIE);
			this->SetState(KOOPA_STATE_DIE);
			mario->AddPoint(100);
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE) {
			//DebugOut(L"[INFO] Koopa moving shell collides with another Koopa\n");
			this->SetState(KOOPA_STATE_DIE);
			mario->AddPoint(100);
		}
	}
}

void CKoopa::OnCollisionWithShinyBrick(LPCOLLISIONEVENT e)
{
	CShinyBrick* shinyBrick = dynamic_cast<CShinyBrick*>(e->obj);
	if (e->nx != 0) shinyBrick->Activate();
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
	else if (state == KOOPA_STATE_FLY)
		aniId = KOOPA_ANI_FLY;
	else if (state == KOOPA_STATE_DIE)
		aniId = KOOPA_ANI_DIE;
	
	if (aniId != -1)
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CKoopa::SetState(int state) {
	CGameObject::SetState(state);

	switch (state)
	{
	case KOOPA_STATE_WALKING_LEFT:
		//DebugOut(L"[INFO] Koopa is walking left\n");
		vx = -KOOPA_WALKING_SPEED;
		ay = KOOPA_GRAVITY;				// naturally fall down when Koopa is out of platform or box
		break;
	case KOOPA_STATE_WALKING_RIGHT:
		//DebugOut(L"[INFO] Koopa is walking right\n");
		vx = KOOPA_WALKING_SPEED;
		ay = KOOPA_GRAVITY;				// naturally fall down when Koopa is out of platform or box
		break;
	case KOOPA_STATE_SHELL_IDLE:
		//DebugOut(L"[INFO] Koopa is shell idle\n");
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_MOVE:
		//DebugOut(L"[INFO] Koopa is in shell move\n");
		ay = KOOPA_GRAVITY;				// naturally fall down when Koopa is out of platform or box
		vx = 0;						
		break;
	case KOOPA_STATE_SHELL_SHAKING:
		//DebugOut(L"[INFO] Koopa is shell shaking\n");
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_IDLE:
		//DebugOut(L"[INFO] Koopa is shell reverse idle\n");
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_MOVE:
		//DebugOut(L"[INFO] Koopa is shell reverse move\n");
		ay = KOOPA_GRAVITY;				// naturally fall down when Koopa is out of platform or box
		break;
	case KOOPA_STATE_SHELL_REVERSE_SHAKING:
		//DebugOut(L"[INFO] Koopa is shell reverse shaking\n");
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_JUMP:
		//DebugOut(L"[INFO] Koopa is shell reverse jump\n");
		vy = -KOOPA_DEFLECT_SPEED;      
		ay = KOOPA_GRAVITY;             
		vx = -vx;                      
		break;
	case KOOPA_STATE_FLY:
		ay = 0;                         
		vx = 0;          

		if (flyUpperY == 0 && flyLowerY == 0) {
			flyUpperY = y;     
			flyLowerY = y + KOOPA_FLY_HEIGHT;  
		}

		isFlyingUp = false;                // Start by flying down
		vy = KOOPA_FLY_SPEED;         
		break;
	case KOOPA_STATE_DIE:
		//DebugOut(L"[INFO] Koopa is dead\n");
		die_start = GetTickCount64();
		ay = KOOPA_GRAVITY;
		break;
	default:
		break;
	}
}

bool CKoopa::IsPlatformEdge(float edgeCheckDistance, vector<LPGAMEOBJECT>& coObjects)
{
	const float VERTICAL_TOLERANCE = 2.5f;   // khoảng dung sai theo chiều dọc
	const float HORIZONTAL_TOLERANCE = 2.0f; // khoảng dung sai theo chiều ngang

	float koopaLeft, koopaTop, koopaRight, koopaBottom;
	GetBoundingBox(koopaLeft, koopaTop, koopaRight, koopaBottom);
	float direction = (state == KOOPA_STATE_WALKING_LEFT) ? -1.0f : 1.0f;

	// find all platforms the Koopa is currently standing on
	vector<LPGAMEOBJECT> platforms;
	for (const auto& obj : coObjects)
	{
		if (obj == this || obj->IsDeleted() || !obj->IsBlocking()) continue;

		float platformLeft, platformTop, platformRight, platformBottom;
		obj->GetBoundingBox(platformLeft, platformTop, platformRight, platformBottom);

		if (koopaLeft < platformRight && koopaRight > platformLeft && // horizontal overlap
			koopaBottom >= platformTop - VERTICAL_TOLERANCE 
			&& koopaBottom <= platformTop + VERTICAL_TOLERANCE) // define the range to determine when Koopa is on platform [platformTop - VERTICAL_TOLERANCE ; platformTop + VERTICAL_TOLERANCE]
			platforms.push_back(obj);
	}

	if (platforms.empty()) return false;

	// calculate the combined bounding box of all platforms
	float leftmostEdge = FLT_MAX;
	float rightmostEdge = -FLT_MAX;
	float platformHeight = 0.0f;

	for (const auto& platform : platforms)
	{
		float platformLeft, platformTop, platformRight, platformBottom;
		platform->GetBoundingBox(platformLeft, platformTop, platformRight, platformBottom);

		if (leftmostEdge == FLT_MAX) // first platform, eg: first brick of many consecutive bricks
		{
			leftmostEdge = platformLeft;
			rightmostEdge = platformRight;
			platformHeight = platformTop;
			continue;
		}

		if (std::abs(platformTop - platformHeight) <= VERTICAL_TOLERANCE) // vẫn nằm trong khoảng dung sai chiều dọc => Koopa vẫn đang đứng trên Platform
		{
			leftmostEdge = min(leftmostEdge, platformLeft);
			rightmostEdge = max(rightmostEdge, platformRight);
		}
	}

	// check when Koopa reach the platform edge 
	// direction * edgeCheckDistance là khoảng cách Koopa lòi ra so với platformEdge
	float projectedX = x + direction * edgeCheckDistance;
	if (direction < 0 && projectedX <= leftmostEdge + 0.001f) return true;
	if (direction > 0 && projectedX >= rightmostEdge - 0.001f) return true;

	return false;
}

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CMario* player = GetPlayer();

	if (state == KOOPA_STATE_DIE && player && player->GetKoopa() == this) {
		player->SetKoopa(nullptr); 
	}

	vx += ax * dt; 
	vy += ay * dt; 

	ULONGLONG now = GetTickCount64();

	isInWall = false; 

	if (state == KOOPA_STATE_WALKING_LEFT) {
		if (IsPlatformEdge(KOOPA_PLATFORM_EDGE_DISTANCE, *coObjects)) {
			SetState(KOOPA_STATE_WALKING_RIGHT);
		}
	}
	else if (state == KOOPA_STATE_WALKING_RIGHT) {
		if (IsPlatformEdge(KOOPA_PLATFORM_EDGE_DISTANCE, *coObjects)) {
			SetState(KOOPA_STATE_WALKING_LEFT);
		}
	}

	switch (state) 
	{
	case KOOPA_STATE_FLY:
		if (!isFlyingUp && y >= flyLowerY) { // reached bottom position
			isFlyingUp = true;
			vy = -KOOPA_FLY_SPEED;  // flying up
		}
		else if (isFlyingUp && y <= flyUpperY) { // reached top position
			isFlyingUp = false;
			vy = KOOPA_FLY_SPEED;  // flying down
		}
		break;
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

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CKoopa::Reload()
{
	CGameObject::Reload();

	this->ax = 0;
	this->ay = KOOPA_GRAVITY;

	if (x == FLYING_KOOPA_ORIGINAL_X
		&& y == FLYING_KOOPA_ORIGINAL_Y)
		SetState(KOOPA_STATE_FLY);
	else SetState(KOOPA_STATE_WALKING_LEFT);

	stateShellStart = -1;
	stateShakingStart = -1;
	die_start = -1;

	isHeld = false;
	isDeleted = false;
	isActive = true;
	isFlyingUp = false;
}
