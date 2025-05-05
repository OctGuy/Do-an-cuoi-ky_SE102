#include "ShinyBrick.h"

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

    // Special activation condition will be checked here in the future
    if (IsActivationConditionMet())
    {
        ActivateItem();
    }

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

void CShinyBrick::ActivateItem()
{
    //if (item == NULL) return;

    //// Same implementation as parent class
    //item->SetPosition(x, y);
    //item->SetActive(true);
    //item = NULL;
}

bool CShinyBrick::IsActivationConditionMet()
{
    // Placeholder function to be implemented later
    // For now, always return true so item activates
    return true;
}
