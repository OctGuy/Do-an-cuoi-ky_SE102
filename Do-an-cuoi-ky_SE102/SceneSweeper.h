#pragma once
#include "Game.h"
#include "GameObject.h"
#include "Wall.h"
#include "PlayScene.h"

#define SCENE_SWEEPER_SPEED 0.04f

using namespace std;

class CSceneSweeper : public CGameObject
{
private:
	LPGAMEOBJECT wall;
	float rightLimit;
public:
	CSceneSweeper(float x, float y, float rightLimit = 0) : CGameObject(x, y) 
	{
		this->wall = nullptr;
		this->rightLimit = rightLimit;

		vx = SCENE_SWEEPER_SPEED;

		CGame* game = CGame::GetInstance();
		CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
		vector<LPGAMEOBJECT>& allObjects = playScene->GetObjects();
		for (auto obj : allObjects)
		{
			if (dynamic_cast<CWall*>(obj))
			{
				this->wall = obj;
				break;
			}
		}
		if (this->wall)
			DebugOut(L"Wall Found");
		else
			DebugOut(L"Wall Not Found");
	}
	~CSceneSweeper() {}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render()
	{

	}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetActive(bool isActive) override { this->isActive = true; }

	void Reload() override;

	//void SetActive(bool isActive) override;

	void SetWall(LPGAMEOBJECT wall) { this->wall = wall; }
	int IsCollidable() { return 1; }
	int IsBlocking() { return 0; }
	int IsTangible() { return 0; }
};