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

void CPipe::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (this->entity == NULL) return;
	CGame* game = CGame::GetInstance();
	CPlayScene* playScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	if (playScene)
	{
		CMario* mario = dynamic_cast<CMario*>(playScene->GetPlayer());
		if (mario)
		{
			float marioX, marioY;
			mario->GetPosition(marioX, marioY);
			if (abs(this->x - marioX) < PIPE_DETECTION_RANGE &&
				abs(this->y - marioY) < PIPE_DETECTION_RANGE)
			{
				//THE ORDER OF THESE FUNCTIONS MATTER
				this->entity->SetPosition(this->x + 8, this->y - 1); // Set the entity position to the pipe position
				this->entity->SetActive(true); // Set the entity active
				this->entity->SetState(100); //100 is the actice state of the entity (lazy implementation)
				this->entity = NULL; // Set the entity to NULL to prevent access error 
			}
		}
	}
}

void CPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - this->cellWidth / 2;
	t = y - this->cellHeight / 2;
	r = l + 2 * this->cellWidth; // pipe is always 2 cells wide
	b = t + this->height * this->cellHeight;
}

int CPipe::IsDirectionColliable(float nx, float ny)
{
	return 1;
}
