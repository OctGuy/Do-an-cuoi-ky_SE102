#include "BoxPlatform.h"

void CBoxPlatform::Render()
{
    if (this->length <= 0 || this->width <= 0) return;
    //DebugOut(L"Rendering Box");
    float yy = y;
    RenderLayer(this->spriteIdTL, this->spriteIdMT, this->spriteIdTR, this->spriteIdSOTCorner, yy);
    yy += this->cellHeight;
    if (this->width > 2) {
        for (int i = 1; i < this->width - 1; i++)
        {
            RenderLayer(this->spriteIdML, this->spriteIdFill, this->spriteIdMR, this->spriteIdSOTBody, yy);
            yy += this->cellHeight;
        }
    }

    if (this->width > 1)
        RenderLayer(this->spriteIdBL, this->spriteIdMB, this->spriteIdBR, this->spriteIdSOTBottom, yy);
    //RenderBoundingBox();  // Add this line to display the bounding box
}

void CBoxPlatform::RenderLayer(int leftLayerId, int midLayerId, int rightLayerId, int spriteIdSOT, float yy)
{
    float xx = x;
    CSprites* s = CSprites::GetInstance();

    // Draw left sprite if ID is not -1
    if (leftLayerId != -1) {
        s->Get(leftLayerId)->Draw(xx, yy);
    }
    xx += this->cellWidth;

    // Render middle of layer if ID is not -1
    if (midLayerId != -1) {
        for (int i = 1; i < this->length - 1; i++)
        {
            s->Get(midLayerId)->Draw(xx, yy);
            xx += this->cellWidth;
        }
    }
    else {
        // Skip the rendering but still update the x position
        xx += this->cellWidth * (this->length - 2);
    }

    // Draw right sprite if ID is not -1 and length > 1
    if (rightLayerId != -1 && length > 1) {
        s->Get(rightLayerId)->Draw(xx, yy);
    }
    xx += this->cellWidth;

    // Draw shadow sprite if ID is not -1
    if (spriteIdSOT != -1) {
        s->Get(spriteIdSOT)->Draw(xx, yy);
    }
}

void CBoxPlatform::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - this->cellWidth / 2;
	t = y - this->cellHeight / 2;
	r = l + this->cellWidth * this->length;
	b = t + this->cellHeight * this->width;
}

void CBoxPlatform::RenderBoundingBox()
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
    // Center the bounding box on the platform
    float xx = l + (r - l) / 2;
    float yy = t + (b - t) / 2;
    CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right, rect.bottom);
}

int CBoxPlatform::IsDirectionColliable(float nx, float ny)
{
    if (nx == 0 && ny == -1) return 1;
    else return 0;
}