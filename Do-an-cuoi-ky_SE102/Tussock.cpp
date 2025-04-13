#include "Tussock.h"

void CTussock::Render() {
	if (this->length <= 0 || this->width <= 0) return;
	
	CSprites* s = CSprites::GetInstance();
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < length; j++) {
			int spriteID = spriteIDs[i][j];
			if (spriteID != -1) { // Check if the sprite ID is valid
				s->Get(spriteID)->Draw(x + cellWidth / 2 + j * cellWidth, y + i * cellHeight);
			}
		}
	}
	//RenderBoundingBox();
}

void CTussock::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x - cellWidth / 2;
	t = y - cellHeight / 2;
	r = l + length * cellWidth;
	b = t + width * cellHeight;
}

void CTussock::RenderBoundingBox() {
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

	// This would center the bounding box on the platform
	float xx = l + (r - l) / 2;
	float yy = t + (b - t) / 2;
	CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right, rect.bottom);
}

int CTussock::IsDirectionColliable(float nx, float ny) {
	return 0;
}