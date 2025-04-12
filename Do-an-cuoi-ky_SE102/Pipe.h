#pragma once
#include "GameObject.h"

class CPipe : public CGameObject
{
protected:
	int height;	
	float cellWidth;
	float cellHeight;
	int spriteIdTL, spriteIdTR, spriteIdBL, spriteIdBR;

public:
	CPipe(float x, float y,
		float cell_width, float cell_height, int height,
		int sprite_id_tl, int sprite_id_tr, int sprite_id_bl, int sprite_id_br) :CGameObject(x, y)
	{
		this->height = height;
		this->cellWidth = cell_width;
		this->cellHeight = cell_height;
		this->spriteIdTL = sprite_id_tl;
		this->spriteIdTR = sprite_id_tr;
		this->spriteIdBL = sprite_id_bl;
		this->spriteIdBR = sprite_id_br;
	}

	void Render() override;
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b) override;
	//void RenderBoundingBox() ;
	int IsDirectionColliable(float nx, float ny) override;
};