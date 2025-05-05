#pragma once
#include "GameObject.h"
#include "ShinyBrick.h"
#include "Coin.h"
#include <vector>

#define ANI_ID_PSWTICH_ACTIVATED 15001
#define ANI_ID_PSWTICH_UNACTIVATED 15000

class CPSwitch : public CGameObject
{
private:
    bool isActivated; // To track if the switch has been activated
    ULONGLONG activationTime = -1; // Time when the switch was activated
    const DWORD SWITCH_DURATION = 10000; // 10 seconds duration
	int numOfActivation = 0; // Track the number of activations

public:
    CPSwitch(float x, float y);

    void Activate(); // Activate the switch
    void Update(DWORD dt, std::vector<LPGAMEOBJECT>* coObjects) override;
    void Render() override;
    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;

	void SwitchCoinBricks(std::vector<LPGAMEOBJECT>* coObjects);

    int IsCollidable() override { return 1; } // Collidable
    int IsBlocking() override { return 0; }  // Not blocking
};
