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
		SwitchCoinBricks(); // Switch coins and bricks
    }
	if (activationTime && currentTime - activationTime > SWITCH_DURATION)
	{
		DebugOut(L"[INFO] 10 second passed\n");
        numOfActivation++;
		isActivated = false; // Deactivate the switch
		SwitchCoinBricks(); // Switch coins and bricks
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

	//RenderBoundingBox();
}

void CPSwitch::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - 8.f;
    top = y - 8.f;
    right = left + 16.f; // Example width
    bottom = top + 16.f; // Example height
}

void CPSwitch::SetActive(bool isActive)
{
    if (this->isActivated == true)
    {
        this->isActive = true; // This mean that the P switch wont be unloaded when it is activated to ensure it switch coin and brick correctly
    }
    else
		CGameObject::SetActive(isActive); // Call the base class method
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

void CPSwitch::SwitchCoinBricks()
{
    CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
    if (!playScene) return;

    // Create a temporary vector to hold new objects (to avoid modifying while iterating and create an infinite loop)
    vector<LPGAMEOBJECT> newObjects;

    vector<LPGAMEOBJECT>& allObjects = playScene->GetObjects();

    for (LPGAMEOBJECT obj : allObjects)
    {
        if (!obj || obj->IsDeleted()) continue; // Skip null or deleted objects

        if (dynamic_cast<CCoin*>(obj)) 
        {
            float cx, cy;
            obj->GetPosition(cx, cy);
            obj->Delete(); 

            // Create a shiny brick and add to new objects list
            LPGAMEOBJECT shinyBrick = new CShinyBrick(cx, cy, 30);
            newObjects.push_back(shinyBrick);
        }
        else if (dynamic_cast<CShinyBrick*>(obj)) 
        {
            float bx, by;
            obj->GetPosition(bx, by);
            obj->Delete(); 

            // Create a coin and add to new objects list
            LPGAMEOBJECT coin = new CCoin(bx, by);
            newObjects.push_back(coin);
        }
    }

    // Add all new objects to the scene
    for (LPGAMEOBJECT newObj : newObjects)
    {
        playScene->Add(newObj);
    }
}