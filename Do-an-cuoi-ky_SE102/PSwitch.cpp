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
		//isActivated = false; // Deactivate the switch
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

void CPSwitch::SwitchCoinBricks()
{
    CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
    if (!playScene) return;

    // Create a temporary vector to hold new objects (to avoid modifying while iterating and create an infinite loop)
    vector<LPGAMEOBJECT> newObjects;

    vector<LPGAMEOBJECT>& allObjects = playScene->GetObjects();

    for (LPGAMEOBJECT obj : allObjects)
    {
        if (!obj || obj->IsDeleted()) continue; 

		if (dynamic_cast<CCoin*>(obj)) //Replace coin with shiny brick
        {
            float cx, cy;
            obj->GetPosition(cx, cy);
            obj->Delete(); 

            LPGAMEOBJECT shinyBrick = new CShinyBrick(cx, cy, 30);
            newObjects.push_back(shinyBrick);
        }
		else if (dynamic_cast<CShinyBrick*>(obj)) //Replace shint brick with coin
        {
            float bx, by;
            obj->GetPosition(bx, by);
            obj->Delete(); 

            LPGAMEOBJECT coin = new CCoin(bx, by);
            newObjects.push_back(coin);
        }
    }

    for (LPGAMEOBJECT newObj : newObjects)
    {
        playScene->Add(newObj);
    }
}