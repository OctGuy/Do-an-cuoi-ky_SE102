#include "PSwitch.h"
#include "Game.h"

CPSwitch::CPSwitch(float x, float y)
    : CGameObject(x, y), isActivated(false), activationTime(0)
{
}

void CPSwitch::Activate()
{
    if (isActivated) return; // Prevent multiple activations
    isActivated = true;
    activationTime = GetTickCount64(); // Record the activation time
}

void CPSwitch::Update(DWORD dt, std::vector<LPGAMEOBJECT>* coObjects)
{
    ULONGLONG currentTime = GetTickCount64();
    if (numOfActivation > 1) return; //Pswitch can only activate 2 times
	if (isActivated && numOfActivation < 1)
    {
		DebugOut(L"[INFO] PSwitch is activated\n");
        numOfActivation++;
		SwitchCoinBricks(coObjects); // Switch coins and bricks
    }
	if (activationTime && currentTime - activationTime > SWITCH_DURATION)
	{
		DebugOut(L"[INFO] 10 second passed\n");
        numOfActivation++;
		SwitchCoinBricks(coObjects); // Switch coins and bricks
	}
}

void CPSwitch::Render()
{
    if (isActivated)
    {
        CAnimations::GetInstance()->Get(ANI_ID_PSWTICH_ACTIVATED)->Render(x, y);
    }
    else
    {
        CAnimations::GetInstance()->Get(ANI_ID_PSWTICH_UNACTIVATED)->Render(x, y);
    }

	RenderBoundingBox();
}

void CPSwitch::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - 8.f;
    top = y - 8.f;
    right = left + 16.f; // Example width
    bottom = top + 16.f; // Example height
}

void CPSwitch::SwitchCoinBricks(std::vector<LPGAMEOBJECT>* coObjects)
{
    CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
    for (LPGAMEOBJECT obj : *coObjects)
    {
        if (dynamic_cast<CCoin*>(obj)) // If it's a coin
        {
            float cx, cy;
            obj->GetPosition(cx, cy);
            obj->Delete(); // Mark the coin for deletion

            LPGAMEOBJECT shinyBrick = new CShinyBrick(cx, cy, 30); // Create a shiny brick
            playScene->Add(shinyBrick); // Add the shiny brick to the game
        }
        else if (dynamic_cast<CShinyBrick*>(obj)) // If it's a shiny brick
        {
            float bx, by;
            obj->GetPosition(bx, by);
            obj->Delete(); // Mark the shiny brick for deletion

            LPGAMEOBJECT coin = new CCoin(bx, by); // Create a coin
            playScene->Add(coin); // Add the coin to the game
        }
    }
}
