#include "TunnelBlock.h"
#include "Game.h"

void CTunnelBlock::Render()
{
    RenderBoundingBox();
}

void CTunnelBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - TUNNEL_BLOCK_BBOX_WIDTH/2;
    t = y - TUNNEL_BLOCK_BBOX_HEIGHT/2;
    r = l + TUNNEL_BLOCK_BBOX_WIDTH;
    b = t + TUNNEL_BLOCK_BBOX_HEIGHT;
}
