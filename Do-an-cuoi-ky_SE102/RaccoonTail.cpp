#include "RaccoonTail.h"
#include "Collision.h"

void CRaccoonTail::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (state == RACCOON_TAIL_STATE_ACTIVE)
    {
        // Move horizontally around the x-axis
        x += vx * dt;

        // Reverse direction if it exceeds the range
        if (x > startX + RACCOON_TAIL_RANGE)
        {
            vx = -vx;
        }
		else if (x < startX - RACCOON_TAIL_RANGE)
		{
			SetState(RACCOON_TAIL_STATE_INACTIVE); // Set inactive state if out of range
		}
    }
    else if (state == RACCOON_TAIL_STATE_INACTIVE)
    {
        isActive = false; // Set inactive status
    }
}

void CRaccoonTail::Render()
{
    // Render bounding box for debugging purposes
    RenderBoundingBox();
}

void CRaccoonTail::SetState(int state)
{
    CGameObject::SetState(state);

    switch (state)
    {
    case RACCOON_TAIL_STATE_ACTIVE:
        isActive = true;
		startX = x; // Store the initial position
        vx = RACCOON_TAIL_SPEED; // Set horizontal speed
        break;

    case RACCOON_TAIL_STATE_INACTIVE:
        isActive = false;
        vx = 0; // Stop movement
        break;
    }
}

void CRaccoonTail::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x ;
    top = y;
    right = x + 16; // Example width
    bottom = y + 16; // Example height
}
