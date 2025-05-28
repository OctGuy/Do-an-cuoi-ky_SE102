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

	if (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT) {
		if (e->nx != 0 && e->obj->IsBlocking()) {
			if (e->nx > 0) {
				SetState(KOOPA_STATE_WALKING_RIGHT);
			}
			else {
				SetState(KOOPA_STATE_WALKING_LEFT);
			}
		}
	}

	if (e->nx == 0 && e->ny == 0 && e->obj->IsBlocking())
	{
		isInWall = true;
	}

	if (state == KOOPA_STATE_SHELL_MOVE || state == KOOPA_STATE_SHELL_REVERSE_MOVE) {
		if (e->nx != 0 && e->obj->IsBlocking()) {
			if (e->nx > 0) {
				vx = KOOPA_SHELL_SPEED;
			}
			else {
				vx = -KOOPA_SHELL_SPEED;
			}
		}

		if (dynamic_cast<CShinyBrick*>(e->obj))
		{
			OnCollisionWithShinyBrick(e);
		}
		else if (dynamic_cast<CQuestionBrick*>(e->obj)) {
			OnCollisionWithBrick(e);
		}	
	}
}

void CKoopa::OnCollisionWithBrick(LPCOLLISIONEVENT e) {
	CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);
	questionBrick->OnCollisionWith(e);
}

void CKoopa::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {
	CMario* mario = GetPlayer();
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

	if (koopa) {
		// ParaTroopa collides with another ParaTroopa
		if (koopa->GetIsHeld()) {
			DebugOut(L"[INFO] Koopa is held by Mario collide with another Koopa\n");
			koopa->SetState(KOOPA_STATE_DIE);
			this->SetState(KOOPA_STATE_DIE);
			mario->AddPoint(100);
		}
		else if (koopa->GetState() == KOOPA_STATE_SHELL_MOVE
			|| koopa->GetState() == KOOPA_STATE_SHELL_REVERSE_MOVE) {
			DebugOut(L"[INFO] Koopa moving shell collides with another Koopa\n");
			this->SetState(KOOPA_STATE_DIE);
			mario->AddPoint(100);
		}
	}
}

void CKoopa::OnCollisionWithShinyBrick(LPCOLLISIONEVENT e)
{
	CShinyBrick* shinyBrick = dynamic_cast<CShinyBrick*>(e->obj);
	shinyBrick->Activate();
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

	RenderBoundingBox();
}

void CKoopa::SetState(int state) {
	CGameObject::SetState(state);

	switch (state)
	{
	case KOOPA_STATE_WALKING_LEFT:
		DebugOut(L"[INFO] Koopa is walking left\n");
		vx = -KOOPA_WALKING_SPEED;
		ay = KOOPA_GRAVITY;				// naturally fall down when Koopa is out of platform or box
		break;
	case KOOPA_STATE_WALKING_RIGHT:
		DebugOut(L"[INFO] Koopa is walking right\n");
		vx = KOOPA_WALKING_SPEED;
		ay = KOOPA_GRAVITY;				// naturally fall down when Koopa is out of platform or box
		break;
	case KOOPA_STATE_SHELL_IDLE:
		DebugOut(L"[INFO] Koopa is shell idle\n");
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_MOVE:
		DebugOut(L"[INFO] Koopa is in shell move\n");
		ay = KOOPA_GRAVITY;				// naturally fall down when Koopa is out of platform or box
		vx = 0;						
		break;
	case KOOPA_STATE_SHELL_SHAKING:
		DebugOut(L"[INFO] Koopa is shell shaking\n");
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_IDLE:
		DebugOut(L"[INFO] Koopa is shell reverse idle\n");
		stateShellStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_MOVE:
		DebugOut(L"[INFO] Koopa is shell reverse move\n");
		ay = KOOPA_GRAVITY;				// naturally fall down when Koopa is out of platform or box
		break;
	case KOOPA_STATE_SHELL_REVERSE_SHAKING:
		DebugOut(L"[INFO] Koopa is shell reverse shaking\n");
		stateShakingStart = GetTickCount64();
		vx = 0;
		break;
	case KOOPA_STATE_SHELL_REVERSE_JUMP:
		DebugOut(L"[INFO] Koopa is shell reverse jump\n");
		vy = -KOOPA_DEFLECT_SPEED;      
		ay = KOOPA_GRAVITY;             
		vx = -vx;                      
		break;
	case KOOPA_STATE_FLY:
		ay = 0;                         
		vx = 0;                         
		// Initialize flying parameters if not set
		if (flyUpperY == 0 && flyLowerY == 0) {
			flyUpperY = y;     // Fly up by KOOPA_FLY_RANGE
			flyLowerY = y + KOOPA_FLY_HEIGHT;  // Fly down by KOOPA_FLY_RANGE
		}
		isFlyingUp = false;                // Start by flying down
		vy = KOOPA_FLY_SPEED;         // Initial downward velocity
		break;
	case KOOPA_STATE_DIE:
		DebugOut(L"[INFO] Koopa is dead\n");
		die_start = GetTickCount64();
		ay = KOOPA_GRAVITY;
		break;
	default:
		break;
	}
}

bool CKoopa::IsPlatformEdge(float checkDistance, vector<LPGAMEOBJECT>& coObjects)
{
	float verticalTolerance = 2.5f;   // Use consistent tolerance (was 2.5f in provided code)
	float horizontalTolerance = 2.0f; // For adjacency
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	float koopaBottomY = b;
	float direction = (state == KOOPA_STATE_WALKING_LEFT) ? -1.0f : 1.0f;

	// Find all platforms the Koopa is currently standing on
	vector<LPGAMEOBJECT> supportingPlatforms;
	for (const auto& obj : coObjects)
	{
		if (obj == this || obj->IsDeleted() || !obj->IsBlocking()) continue;

		float objL, objT, objR, objB;
		obj->GetBoundingBox(objL, objT, objR, objB);

		if (l < objR && r > objL && // Horizontal overlap
			koopaBottomY >= objT - verticalTolerance && koopaBottomY <= objT + verticalTolerance) // Vertical proximity
		{
			supportingPlatforms.push_back(obj);
		}
	}

	if (supportingPlatforms.empty())
	{
		// DebugOut(L"[INFO] No supporting platform found for Koopa at Y_bottom=%f\n", koopaBottomY);
		return false;
	}

	// Find consecutive platforms
	float combinedLeft = FLT_MAX;
	float combinedRight = -FLT_MAX;
	float combinedTop = 0.0f;

	for (const auto& platform : supportingPlatforms)
	{
		float platformL, platformT, platformR, platformB;
		platform->GetBoundingBox(platformL, platformT, platformR, platformB);

		if (combinedLeft == FLT_MAX)
		{
			combinedLeft = platformL;
			combinedRight = platformR;
			combinedTop = platformT;
			continue;
		}

		if (std::abs(platformT - combinedTop) <= verticalTolerance)
		{
			combinedLeft = min(combinedLeft, platformL);
			combinedRight = max(combinedRight, platformR);
		}
	}

	// Check edge
	float projectedX = x + direction * checkDistance;
	if (direction < 0)
	{
		if (projectedX <= combinedLeft + 0.001f) return true;
	}
	else
	{
		if (projectedX >= combinedRight - 0.001f) return true;
	}

	return false;
}

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CMario* player = GetPlayer();

	if (state == KOOPA_STATE_DIE && player && player->GetKoopa() == this) {
		player->SetKoopa(nullptr); 
	}

	// flying logic
	//if (state == KOOPA_STATE_FLY) {
	//	
	//}
	//else { // normal physics for non-flying states
	//	vy += ay * dt;
	//	vx += ax * dt;
	//}

	vx += ax * dt; // Apply acceleration to velocity
	vy += ay * dt; // Apply gravity to vertical velocity

	ULONGLONG now = GetTickCount64();

	isInWall = false; 

	if (state == KOOPA_STATE_WALKING_LEFT) {
		if (IsPlatformEdge(0.1f, *coObjects)) {
			SetState(KOOPA_STATE_WALKING_RIGHT);
		}
	}
	else if (state == KOOPA_STATE_WALKING_RIGHT) {
		if (IsPlatformEdge(0.1f, *coObjects)) {
			SetState(KOOPA_STATE_WALKING_LEFT);
		}
	}

	switch (state) 
	{
	case KOOPA_STATE_FLY:
		//DebugOut(L"[INFO] Koopa position: %f, %f\n", x, y);
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
