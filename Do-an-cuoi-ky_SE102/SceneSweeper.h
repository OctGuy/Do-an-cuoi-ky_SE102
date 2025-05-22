#pragma once
#include "Game.h"
#include "GameObject.h"

#define SCENE_SWEEPER_SPEED 0.04f

using namespace std;

class CSceneSweeper : public CGameObject
{
public:
	CSceneSweeper(float x, float y) : CGameObject(x, y) 
	{
		DebugOut(L"[INFO] SceneSweeper Position: %f\n", x);
	}
	~CSceneSweeper() {}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = nullptr) override;
	void Render() {};
	void GetBoundingBox(float& l, float& t, float& r, float& b) override;
	void SetActive(bool isActive) override { this->isActive = true; }

	//void SetActive(bool isActive) override;

	int IsCollidable() { return 1; }
	int IsBlocking() { return 0; }
	int IsTangible() { return 0; }
};