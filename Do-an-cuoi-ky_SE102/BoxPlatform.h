#pragma once
#include "GameObject.h"
#include "debug.h"
#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"

class CBoxPlatform : public CGameObject
{
protected:
	int length;				// Unit: cell 
	int width;
	float cellWidth;
	float cellHeight;
	int spriteIdTL, spriteIdTR, spriteIdBL, spriteIdBR, spriteIdFill; // Sprite ID of 4 corners and fill
	int spriteIdMT, spriteIdMB, spriteIdML, spriteIdMR; // Sprite ID of 4 middle edges

public:
	CBoxPlatform(float x, float y, int length, int width, 
		float cellWidth, float cellHeight, int spriteIdTL, int spriteIdTR, 
		int spriteIdBL, int spriteIdBR, int spriteIdFill, 
		int spriteIdMT, int spriteIdMB, int spriteIdML, int spriteIdMR) :CGameObject(x, y) 
	{
		this->length = length;
		this->width = width;
		this->cellWidth = cellWidth;
		this->cellHeight = cellHeight;
		this->spriteIdTL = spriteIdTL;
		this->spriteIdTR = spriteIdTR;
		this->spriteIdBL = spriteIdBL;
		this->spriteIdBR = spriteIdBR;
		this->spriteIdFill = spriteIdFill;

		this->spriteIdMT = spriteIdMT;
		this->spriteIdMB = spriteIdMB;
		this->spriteIdML = spriteIdML;
		this->spriteIdMR = spriteIdMR;
	}

	void Render();
	//Render a layer of the box platform
	void RenderLayer(int leftLayerId, int midLayerId, int rightLayerId, float yy); //yy is the y position of the layer

	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();
	int IsDirectionColliable(float nx, float ny);
};