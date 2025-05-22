#include "ParaTroopa.h"

void CParaTroopa::GetBoundingBox(float& l, float& t, float& r, float& b) {
	if (state == PARATROOPA_STATE_WALKING_LEFT 
		|| state == PARATROOPA_STATE_WALKING_RIGHT
		|| state == PARATROOPA_STATE_BOUNCE_LEFT
		|| state == PARATROOPA_STATE_BOUNCE_RIGHT) {
		l = x - PARATROOPA_BBOX_WIDTH / 2;
		t = y - PARATROOPA_BBOX_HEIGHT / 2;
		r = l + PARATROOPA_BBOX_WIDTH;
		b = t + PARATROOPA_BBOX_HEIGHT;
	}
	else {
		l = x - PARATROOPA_SHELL_BBOX_WIDTH / 2;
		t = y - PARATROOPA_SHELL_BBOX_HEIGHT / 2;
		r = l + PARATROOPA_SHELL_BBOX_WIDTH;
		b = t + PARATROOPA_SHELL_BBOX_HEIGHT;
	}
}

void CParaTroopa::OnNoCollision(DWORD dt) {
	x += vx * dt;
	y += vy * dt;
}

