#include "SceneSweeper.h"
#include "Game.h"

// SceneSweeper.cpp - Update with the correct parameter type
void CSceneSweeper::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	CGame* game = CGame::GetInstance();
	float leftBound, rightBound, bottomBound;
	playScene->GetBoundary(leftBound, rightBound, bottomBound);

	if (x + game->GetBackBufferWidth() > rightLimit)
	{
		vx = 0;
		return;
	}

	//DebugOut(L"[INFO] SceneSweeper Position: %f\n", x);
	x += vx * dt;

	if(wall)
		this->wall->SetPosition(x + game->GetBackBufferWidth() - 8.f, 0.f);

	playScene->SetCamLeftBound(x);
	playScene->SetCamFollowOnX(false);

	float CamX, CamY;
	game->GetCamPos(CamX, CamY);
	game->SetCamPos(x, CamY);
}

void CSceneSweeper::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + 1;
	b = y + 1; 
}

void CSceneSweeper::Reload()
{
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
	playScene->SetCamFollowOnX(true);
	this->Delete();
}

//void CSceneSweeper::SetActive(bool isActive)
//{
//	SetActive(true);
//}
