#pragma once

#include "GameObject.h"

#define PARTICLE_TYPE_HIT 1
#define PARTICLE_TYPE_POINT 2
#define PARTICLE_TYPE_BRICK 3
#define PARTICLE_TYPE_BRICK_LOW 4
#define PARTICLE_TYPE_BRICK_REVERSED 5
#define PARTICLE_TYPE_BRICK_REVERSED_LOW 6
#define PARTICLE_TYPE_SMOKE 7
#define PARTICLE_TYPE_MUSHROOM 8
#define PARTICLE_TYPE_PLANT 9
#define PARTICLE_TYPE_STAR 10
#define PARTICLE_TYPE_TEXT_1 11
#define PARTICLE_TYPE_TEXT_2 12
#define PARTICLE_TYPE_CARD_PLANT 13
#define PARTICLE_TYPE_CARD_MUSHROOM 14
#define PARTICLE_TYPE_CARD_STAR 15

#define PARTICLE_HIT_LIFE_TIME 250
#define PARTICLE_POINT_LIFE_TIME 1000
#define PARTICLE_BRICK_LIFE_TIME 800
#define PARTICLE_SMOKE_LIFE_TIME 240
#define PARTICLE_CARD_LIFE_TIME 300
#define PARTICLE_TEXT_LIFE_TIME 5000

#define ID_ANI_PARTICLE_SMOKE 302
#define ID_ANI_PARTICLE_HIT 12000
#define ID_ANI_PARTICLE_POINT_100 12010
#define ID_ANI_PARTICLE_POINT_200 12011
#define ID_ANI_PARTICLE_POINT_400 12012
#define ID_ANI_PARTICLE_POINT_800 12013
#define ID_ANI_PARTICLE_POINT_1000 12014
#define ID_ANI_PARTICLE_POINT_2000 12015
#define ID_ANI_PARTICLE_POINT_4000 12016
#define ID_ANI_PARTICLE_POINT_8000 12017
#define ID_ANI_PARTICLE_POINT_1UP 12018
#define ID_ANI_PARTICLE_BRICK 12020
#define ID_ANI_PARTICLE_BRICK_REVERSED 12021
#define ID_ANI_PARTICLE_BRICK_LOW 12022
#define ID_ANI_PARTICLE_MUSHROOM 12030
#define ID_ANI_PARTICLE_PLANT 12031
#define ID_ANI_PARTICLE_STAR 12032
#define ID_ANI_PARTICLE_TEXT_1 12040
#define ID_ANI_PARTICLE_TEXT_2 12041
#define ID_ANI_PARTICLE_CARD_PLANT 12043
#define ID_ANI_PARTICLE_CARD_MUSHROOM 12042
#define ID_ANI_PARTICLE_CARD_STAR 12044



class CParticle : public CGameObject
{
protected:
    DWORD lifetime; // Duration the particle will exist
    DWORD creationTime; // Time when the particle was created
	int type; // Type of particle (e.g., smoke, explosion, etc.)
    int point; // To choose the correct point particle to show

public:
    CParticle(float x, float y, int type, int p = 0) : CGameObject(x, y)
    {
        this->creationTime = GetTickCount64();
		this->type = type;
        if (type == PARTICLE_TYPE_POINT) {
			this->vx = 0.f;
			this->vy = -0.05f;
            this->lifetime = PARTICLE_POINT_LIFE_TIME;
			this->point = p;
		}
		else if (type == PARTICLE_TYPE_HIT) {
            this->vx = 0.f;
            this->vy = 0.f;
            this->lifetime = PARTICLE_HIT_LIFE_TIME;
		}
        else if (type == PARTICLE_TYPE_BRICK) {
            this->vx = 0.07f;
            this->vy = -0.26f;
            this->lifetime = PARTICLE_BRICK_LIFE_TIME;
		}
		else if (type == PARTICLE_TYPE_BRICK_LOW) {
			this->vx = 0.07f;
			this->vy = -0.13f;
			this->lifetime = PARTICLE_BRICK_LIFE_TIME;
		}
		else if (type == PARTICLE_TYPE_BRICK_REVERSED) {
			this->vx = -0.07f;
			this->vy = -0.26f;
			this->lifetime = PARTICLE_BRICK_LIFE_TIME;
		}
        else if (type == PARTICLE_TYPE_BRICK_REVERSED_LOW) {
            this->vx = -0.07f;
            this->vy = -0.13f;
            this->lifetime = PARTICLE_BRICK_LIFE_TIME;
        }
        else if (type == PARTICLE_TYPE_SMOKE) {
            this->vx = 0.f;
            this->vy = -0.f;
            this->lifetime = PARTICLE_SMOKE_LIFE_TIME;
        }
		else if (type == PARTICLE_TYPE_MUSHROOM) {
			this->vx = 0.f;
			this->vy = -0.1f;
			this->lifetime = PARTICLE_CARD_LIFE_TIME;
		}
		else if (type == PARTICLE_TYPE_PLANT) {
			this->vx = 0.f;
			this->vy = -0.1f;
			this->lifetime = PARTICLE_CARD_LIFE_TIME;
		}
		else if (type == PARTICLE_TYPE_STAR) {
			this->vx = 0.f;
			this->vy = -0.1f;
			this->lifetime = PARTICLE_CARD_LIFE_TIME;
		}
		else if (type == PARTICLE_TYPE_TEXT_1 ||
				 type == PARTICLE_TYPE_TEXT_2|| 
				 type == PARTICLE_TYPE_CARD_PLANT ||
				 type == PARTICLE_TYPE_CARD_STAR ||
				 type == PARTICLE_TYPE_CARD_MUSHROOM)
		{
			this->vx = 0.f;
			this->vy = 0.f;
			this->lifetime = PARTICLE_TEXT_LIFE_TIME;
		}
		else {
			this->vx = 0.f;
			this->vy = 0.f;
			this->lifetime = 0;
		}
    }

    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = nullptr) override;

    void Render() override;

    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;

    virtual int IsCollidable() override { return 0; } // Non-collidable
    virtual int IsBlocking() override { return 0; }  // Non-blocking
};
