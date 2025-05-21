#include "Pipe.h"

void CPipe::Render()
{
	if (this->height <= 0) return;
	float xx = x;
	float yy = y;
	CSprites* s = CSprites::GetInstance();

	//top part
	s->Get(this->spriteIdTL)->Draw(xx, yy);
	xx += this->cellWidth;
	s->Get(this->spriteIdTR)->Draw(xx, yy);

	//bottom part
	for (int i = 1; i < height; i++) {
		xx = x;
		yy += this->cellHeight;
		s->Get(this->spriteIdBL)->Draw(xx, yy);
		xx += this->cellWidth;
		s->Get(this->spriteIdBR)->Draw(xx, yy);
	}

	//RenderBoundingBox();
}

void CPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	//- 1 and + 2 are expanding bounding box to the left and right 1px
	l = x - this->cellWidth / 2 - 1;
	t = y - this->cellHeight / 2;
	r = l + 2 * this->cellWidth + 2; // pipe is always 2 cells wide
	b = t + this->height * this->cellHeight;
}

int CPipe::IsDirectionColliable(float nx, float ny)
{
	return 1;
}
