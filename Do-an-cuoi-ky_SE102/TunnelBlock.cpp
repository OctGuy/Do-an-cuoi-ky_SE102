#include "TunnelBlock.h"
#include "Game.h"
#include "Mario.h"
#include "PlayScene.h"

void CTunnelBlock::Render()
{
    RenderBoundingBox();
}

void CTunnelBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CTunnelBlock::OnNoCollision(DWORD dt)
{
    return;
}

void CTunnelBlock::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (dynamic_cast<CMario*>(e->obj))
	{
		OnCollisionWithMario(e);
	}
}

void CTunnelBlock::OnCollisionWithMario(LPCOLLISIONEVENT e)
{
    if(e->nx ==0 && e->ny == 0)
    {
        CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
        if (!playScene) return;

        playScene->ActivateAllObjects();

        if (type == TUNNEL_BLOCK_TYPE_ENTER) return;
		playScene->SetCamLockPos(camLockPos); 
		playScene->SetCamBottomBound(camBottomBound);

    }
}


void CTunnelBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - TUNNEL_BLOCK_BBOX_WIDTH/2;
    t = y - TUNNEL_BLOCK_BBOX_HEIGHT/2;
    r = l + TUNNEL_BLOCK_BBOX_WIDTH;
    b = t + TUNNEL_BLOCK_BBOX_HEIGHT;
}
