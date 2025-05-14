#include "Portal.h"
#include "Game.h"
#include "Textures.h"

void CPortal::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPTEXTURE bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	float cx, cy;
	CGame::GetInstance()->GetCamPos(cx, cy);

	CGame::GetInstance()->Draw(x - cx, y - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CPortal::Render()
{
	DebugOut(L"[INFO] Portal position %f %f\n", x, y);
	RenderBoundingBox();
}

void CPortal::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	l = x - PORTAL_BBOX_WIDTH/2;
	t = y - PORTAL_BBOX_HEIGHT/2;
	r = l + PORTAL_BBOX_WIDTH;
	b = t + PORTAL_BBOX_HEIGHT;
}

void CPortal::Reload()
{
	CGameObject::Reload();
}

void CPortal::SetActive(bool isActive)
{
	CGameObject::SetActive(isActive);
	DebugOut(L"[INFO] Portal %d is %s\n", scene_id, this->isActive ? L"active" : L"inactive");
}
