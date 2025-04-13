#pragma once
#include "GameObject.h"
#include "debug.h"
#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"
#include <vector>

class CTussock : public CGameObject
{
protected:
	int length;
	int width;
	float cellWidth;
	float cellHeight;
	vector<vector<int>> spriteIDs; // 2D array of sprite IDs
public:
	CTussock(float x, float y, int length, int width,
		float cellWidth, float cellHeight, vector<vector<int>> spriteIDs) : CGameObject(x, y)
	{
		this->length = length;
		this->width = width;
		this->cellWidth = cellWidth;
		this->cellHeight = cellHeight;
		this->spriteIDs = spriteIDs;
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();
	int IsDirectionColliable(float nx, float ny);
};

