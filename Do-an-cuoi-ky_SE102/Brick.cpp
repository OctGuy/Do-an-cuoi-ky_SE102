#include "Brick.h"

void CBrick::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int id = GetAnimationid();
	if (id == -1)
	{
		return;
	}
	animations->Get(id)->Render(x, y);
	//RenderBoundingBox();
}

int CBrick::GetAnimationid()
{
	if (type == BRICK_TYPE_FLOOR_BOTTOM_LEFT)
		return ID_ANI_BRICK_FLOOR_BOTTOM_LEFT;
	else if (type == BRICK_TYPE_FLOOR_BOTTOM_MIDDLE)
		return ID_ANI_BRICK_FLOOR_BOTTOM_MIDDLE;
	else if (type == BRICK_TYPE_FLOOR_BOTTOM_RIGHT)
		return ID_ANI_BRICK_FLOOR_BOTTOM_RIGHT;
	else if (type == BRICK_TYPE_FLOOR_TOP_LEFT)
		return ID_ANI_BRICK_FLOOR_TOP_LEFT;
	else if (type == BRICK_TYPE_FLOOR_TOP_MIDDLE)
		return ID_ANI_BRICK_FLOOR_TOP_MIDDLE;
	else if (type == BRICK_TYPE_FLOOR_TOP_RIGHT)
		return ID_ANI_BRICK_FLOOR_TOP_RIGHT;
	else if (type == BRICK_TYPE_CONVEX)
		return ID_ANI_BRICK_CONVEX;
	else if (type == BRICK_TYPE_CLOUD)
		return ID_ANI_BRICK_CLOUD;
	else if (type == BRICK_TYPE_QUESTION)
		return ID_ANI_QUESTION_BRICK;
	else if (type == BRICK_TYPE_SHINY)
		return ID_ANI_SHINY_BRICK;
	else
		return -1;

}

void CBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BRICK_BBOX_WIDTH / 2;
	t = y - BRICK_BBOX_HEIGHT / 2;
	r = l + BRICK_BBOX_WIDTH;
	b = t + BRICK_BBOX_HEIGHT;
}