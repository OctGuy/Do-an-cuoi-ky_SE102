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
}

void CPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - this->cellWidth / 2;
	t = y - this->cellHeight / 2;
	r = l + 2 * this->cellWidth; // pipe is alway 2 cells wide
	b = t + this->height * this->cellHeight;
}

int CPipe::IsDirectionColliable(float nx, float ny)
{
	return 1;
}
