#pragma once

#include "GameObject.h"

#define PARTICLE_TYPE_HIT 1
#define PARTICLE_TYPE_POINT 2

#define PARTICLE_HIT_LIFE_TIME 250
#define PARTICLE_POINT_LIFE_TIME 1000

#define ID_ANI_PARTICLE_HIT 12000
#define ID_ANI_PARTICLE_POINT 12010

class CParticle : public CGameObject
{
protected:
    DWORD lifetime; // Duration the particle will exist
    DWORD creationTime; // Time when the particle was created
	int type; // Type of particle (e.g., smoke, explosion, etc.)

public:
    CParticle(float x, float y, int type) : CGameObject(x, y)
    {
        this->creationTime = GetTickCount64();
		this->type = type;
        if (type == PARTICLE_TYPE_POINT) {
			this->vx = 0.f;
			this->vy = -0.05f;
            this->lifetime = PARTICLE_POINT_LIFE_TIME;
		}
		else if (type == PARTICLE_TYPE_HIT) {
            this->vx = 0.f;
            this->vy = 0.f;
            this->lifetime = PARTICLE_HIT_LIFE_TIME;
		}
        else {
            this->vx = 0.f;
            this->vy = 0.f;
			lifetime = 0;
        }
    }

    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = nullptr) override;

    void Render() override;

    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;

    virtual int IsCollidable() override { return 0; } // Non-collidable
    virtual int IsBlocking() override { return 0; }  // Non-blocking
};
