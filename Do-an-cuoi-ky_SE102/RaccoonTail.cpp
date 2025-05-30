#include "RaccoonTail.h"
#include "Collision.h"
#include "Goomba.h" 
#include "Koopa.h"  
#include "ParaTroopa.h"
#include "debug.h" 
#include "QuestionBrick.h"
#include "ShinyBrick.h"
#include "PiranhaPlant.h"
#include "PlainPiranha.h"
#include "WingedGoomba.h"
#include "BoomerangBrother.h"

void CRaccoonTail::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CRaccoonTail::OnCollisionWith(LPCOLLISIONEVENT e)
{
    CParticle* particle = new CParticle(x, y, PARTICLE_TYPE_HIT);
    CPlayScene* currentScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
    CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());

    if (dynamic_cast<CGoomba*>(e->obj)) {
        OnCollisionWithGoomba(e);
        currentScene->Add(particle);
        mario->AddPoint(100, e);
    }
    else if (dynamic_cast<CKoopa*>(e->obj)) {
        OnCollisionWithKoopa(e);
        currentScene->Add(particle);
    }
    else if (dynamic_cast<CParaTroopa*>(e->obj)) {
        OnCollisionWithParaTroopa(e);
        currentScene->Add(particle);
    }
    else if (dynamic_cast<CPiranhaPlant*>(e->obj)) {
        OnCollisionWithPiranhaPlant(e);
        currentScene->Add(particle);
        mario->AddPoint(100, e);
    }
    else if (dynamic_cast<CPlainPiranha*>(e->obj)) {
        OnCollisionWithPlainPiranha(e);
        currentScene->Add(particle);
        mario->AddPoint(100, e);
    }
    else if (dynamic_cast<CWingedGoomba*>(e->obj)) {
        OnCollisionWithWingedGoomba(e);
        currentScene->Add(particle);
        mario->AddPoint(100, e);
    }
    else if (dynamic_cast<CShinyBrick*>(e->obj)) 
    {
        OnCollisionWithShinyBrick(e);
    }
    else if (dynamic_cast<CQuestionBrick*>(e->obj))
    {
        OnCollisionWithQuestionBrick(e);
    }
	else if (dynamic_cast<CBoomerangBrother*>(e->obj))
	{
		CBoomerangBrother* boomerangBrother = dynamic_cast<CBoomerangBrother*>(e->obj);
		boomerangBrother->SetState(BOOMERANG_BROTHER_STATE_DIE);
		currentScene->Add(particle);
		mario->AddPoint(1000, e);
	}
}

void CRaccoonTail::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
    CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
    goomba->SetState(GOOMBA_STATE_DIE_REVERSE);
}

void CRaccoonTail::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e)
{
    CWingedGoomba* wingedGoomba = dynamic_cast<CWingedGoomba*>(e->obj);
    wingedGoomba->SetState(GOOMBA_WING_STATE_DIE_REVERSE);
}

void CRaccoonTail::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e)
{
    CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);
    if (!questionBrick->IsHit())
        questionBrick->Activate();
}

void CRaccoonTail::OnCollisionWithShinyBrick(LPCOLLISIONEVENT e)
{
	CShinyBrick* shinyBrick = dynamic_cast<CShinyBrick*>(e->obj);
	shinyBrick->Activate();
    SetActive(false); //Deactivate so tail wont break multible brick
}

void CRaccoonTail::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
    CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
    koopa->SetSpeed(nx * KOOPA_SHELL_SPEED / 2, 0); // Set speed for Koopa
    koopa->SetState(KOOPA_STATE_SHELL_REVERSE_JUMP);
    SetActive(false);
}

void CRaccoonTail::OnCollisionWithParaTroopa(LPCOLLISIONEVENT e)
{
	CParaTroopa* paraTroopa = dynamic_cast<CParaTroopa*>(e->obj);
	paraTroopa->SetSpeed(nx * PARATROOPA_SHELL_SPEED / 2, 0); // Set speed for ParaTroopa
	paraTroopa->SetState(PARATROOPA_STATE_SHELL_REVERSE_JUMP);
	SetActive(false);
}

void CRaccoonTail::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
    CPiranhaPlant* piranha = dynamic_cast<CPiranhaPlant*>(e->obj);
    piranha->SetState(PIRANHA_STATE_DIE);
}

void CRaccoonTail::OnCollisionWithPlainPiranha(LPCOLLISIONEVENT e)
{
	CPlainPiranha* plainPiranha = dynamic_cast<CPlainPiranha*>(e->obj);
	plainPiranha->SetState(PLAIN_PIRANHA_STATE_DIE);
}

void CRaccoonTail::Render()
{
    // Render bounding box for debugging purposes
    //RenderBoundingBox();
}

void CRaccoonTail::SetState(int state)
{
    CGameObject::SetState(state);

    switch (state)
    {
    case RACCOON_TAIL_STATE_ACTIVE:
		SetActive(true); // Set active state
		startX = x; // Store the initial position
        vx = RACCOON_TAIL_SPEED; // Set horizontal speed
        break;

    case RACCOON_TAIL_STATE_INACTIVE:
		SetActive(false); // Set inactive state
        vx = 0; // Stop movement
        break;
    }
}

void CRaccoonTail::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - RACCOON_TAIL_BBOX_WIDTH;
    t = y - RACCOON_TAIL_BBOX_HEIGHT/2;
    r = l + RACCOON_TAIL_BBOX_WIDTH * 2;
	b = t + RACCOON_TAIL_BBOX_HEIGHT; //8.f is the right height of the tail as to not collide with the ground
}
