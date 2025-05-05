#include "ShinyBrick.h"
#include "Game.h"
#include "PlayScene.h"

void CShinyBrick::Render()
{
    CBrick::Render();
}

void CShinyBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    // Only process collisions if not deleted
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CShinyBrick::Activate()
{
	CGame* game = CGame::GetInstance();
	CPlayScene* scene = (CPlayScene*)game->GetCurrentScene();
	CParticle* brick_bit = new CParticle(x, y, PARTICLE_TYPE_BRICK);
    CParticle* brick_bit_low = new CParticle(x, y, PARTICLE_TYPE_BRICK_LOW);
	CParticle* brick_bit_reversed = new CParticle(x, y, PARTICLE_TYPE_BRICK_REVERSED);
	CParticle* brick_bit_reversed_low = new CParticle(x, y, PARTICLE_TYPE_BRICK_REVERSED_LOW);

	scene->Add(brick_bit);
	scene->Add(brick_bit_low);
	scene->Add(brick_bit_reversed);
	scene->Add(brick_bit_reversed_low);

    // Delete itself immediately after activation
    Delete();
}

void CShinyBrick::SetState(int state)
{
    CGameObject::SetState(state);
    switch (state)
    {
    case BRICK_STATE_BOUNCE:
        // No bouncing, just activate and delete
        Activate();
        break;

    case BRICK_STATE_SHINY_ACTIVATE:
        Activate();
        break;
    }
}
