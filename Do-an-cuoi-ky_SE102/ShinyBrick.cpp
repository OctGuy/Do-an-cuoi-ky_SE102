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
