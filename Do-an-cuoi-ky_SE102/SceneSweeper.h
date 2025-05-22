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
public:
	CSceneSweeper(float x, float y) : CGameObject(x, y) 
	{
		this->wall = nullptr;
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
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	void Render() {};
	void GetBoundingBox(float& l, float& t, float& r, float& b) override;
	void SetActive(bool isActive) override { this->isActive = true; }

	//void SetActive(bool isActive) override;

	int IsCollidable() { return 1; }
	int IsBlocking() { return 0; }
	int IsTangible() { return 0; }
};