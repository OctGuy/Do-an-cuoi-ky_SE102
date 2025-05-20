#pragma once

#include "Texture.h"

class CSprite
{
	int id;				// Sprite ID in the sprite database

	int left;
	int top;
	int right;
	int bottom;

	LPTEXTURE texture;
	D3DX10_SPRITE sprite;
	D3DXMATRIX matScaling;
public:
	CSprite(int id, int left, int top, int right, int bottom, LPTEXTURE tex);

	int GetID() { return id; }
	void Draw(float x, float y, float alpha = 1.0f);
};

typedef CSprite* LPSPRITE;