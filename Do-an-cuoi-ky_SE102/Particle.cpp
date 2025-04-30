#include "Particle.h"

void CParticle::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    // Automatically delete the particle after its lifetime
    if (GetTickCount64() - creationTime >= lifetime)
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
