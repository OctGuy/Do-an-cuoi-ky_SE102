#pragma once
#include "GameObject.h"

#define TUNNEL_BLOCK_BBOX_WIDTH 16
#define TUNNEL_BLOCK_BBOX_HEIGHT 16

#define TUNNEL_BLOCK_TYPE_ENTER 0
#define TUNNEL_BLOCK_TYPE_UP 1
#define TUNNEL_BLOCK_TYPE_DOWN 2

class CTunnelBlock : public CGameObject
{
	float destX;
	float destY;

	int type;

	float camLockPos;
	float camLeftBound;
	float camBottomBound;
	float camRightBound;
public:
	CTunnelBlock(float x, float y, float destX, float destY, int type, 
		float camLockPos, float camLeftBound, float camBottomBound, float camRightBound)
        : CGameObject(x, y)
    {
		this->destX = destX;
		this->destY = destY;

		this->type = type;
		this->camLockPos = camLockPos;
		this->camLeftBound = camLeftBound;
		this->camBottomBound = camBottomBound;
		this->camRightBound = camRightBound;
    }

    void Render() override;
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	void SetActive(bool isActive) { this->isActive = true; }

	void OnNoCollision(DWORD dt) override;
	void OnCollisionWith(LPCOLLISIONEVENT e) override;
	void OnCollisionWithMario(LPCOLLISIONEVENT e);

    void GetBoundingBox(float& l, float& t, float& r, float& b) override;
    int IsCollidable() override { return 1; }
	int IsBlocking() override { return 1; }
	int IsTangible() override { return 0; }
    int IsDirectionColliable(float nx, float ny) override { return 1; }

	void GetDestination(float& x, float& y)
	{
		x = destX;
		y = destY;
	}

	float GetType() { return type; }
};
