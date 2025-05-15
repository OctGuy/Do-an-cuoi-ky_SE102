#pragma once
#include "GameObject.h"

#define TUNNEL_BLOCK_BBOX_WIDTH 16
#define TUNNEL_BLOCK_BBOX_HEIGHT 16

class CTunnelBlock : public CGameObject
{
	float destX;
	float destY;
public:
	CTunnelBlock(float x, float y, float destX, float destY)
        : CGameObject(x, y)
    {
		this->destX = destX;
		this->destY = destY;
    }

    void Render() override;
    void GetBoundingBox(float& l, float& t, float& r, float& b) override;
    int IsCollidable() override { return 1; }
	int IsBlocking() override { return 0; }
	int IsTangible() override { return 0; }
    int IsDirectionColliable(float nx, float ny) override { return 1; }

	void GetDestination(float& x, float& y)
	{
		x = destX;
		y = destY;
	}
};
