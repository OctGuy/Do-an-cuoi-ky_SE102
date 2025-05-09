#pragma once
#include "PowerUp.h"

#define ID_ANI_1UP_MUSHROOM_RISE    14001

#define ID_SPRITE_1UP_MUSHROOM  50003

class C1UpMushroom : public CPowerUp {
public:
    C1UpMushroom(float x, float y) : CPowerUp(x, y) {}

    void Render();

    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
        CPowerUp::Update(dt, coObjects);
    }

    void SetState(int state) {
		CPowerUp::SetState(state);  
    }
};
