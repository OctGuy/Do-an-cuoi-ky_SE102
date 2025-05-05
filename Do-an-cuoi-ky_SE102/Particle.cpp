#include "Particle.h"

void CParticle::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    ULONGLONG now = GetTickCount64();

	if (type == PARTICLE_TYPE_BRICK || type == PARTICLE_TYPE_BRICK_LOW || 
        type == PARTICLE_TYPE_BRICK_REVERSED || type == PARTICLE_TYPE_BRICK_REVERSED_LOW)
	{
		if (now - creationTime >= PARTICLE_BRICK_LIFE_TIME/3)
		{
			vy = 0.26f;
		}
	}

    // Automatically delete the particle after its lifetime
    if (now - creationTime >= lifetime)
    {
        this->Delete();
    }

	x += vx * dt; 
	y += vy * dt; 
}

void CParticle::Render()
{
    int aniID = -1;
	if (type == PARTICLE_TYPE_POINT)
		aniID = ID_ANI_PARTICLE_POINT;
	else if (type == PARTICLE_TYPE_HIT)
		aniID = ID_ANI_PARTICLE_HIT;
	else if (type == PARTICLE_TYPE_BRICK || type == PARTICLE_TYPE_BRICK_LOW)
		aniID = ID_ANI_PARTICLE_BRICK;
	else if (type == PARTICLE_TYPE_BRICK_REVERSED || type == PARTICLE_TYPE_BRICK_REVERSED_LOW)
		aniID = ID_ANI_PARTICLE_BRICK_REVERSED;
	else if (type == PARTICLE_TYPE_SMOKE)
		aniID = ID_ANI_PARTICLE_SMOKE;

	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniID)->Render(x, y);
}

void CParticle::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x;
    top = y;

    if (type == PARTICLE_TYPE_HIT)
    {
        right = left + 16;
        bottom = top + 16;
    }
    else if (type == PARTICLE_TYPE_POINT)
    {
        right = left + 8;
        bottom = top + 8;
    }
    else
    {
        right = left + 8;
        bottom = top + 8;
    }
}
