#pragma once
#include "QuestionBrick.h"

#define BRICK_STATE_SHINY_ACTIVATE 101

class CShinyBrick : public CQuestionBrick
{
public:
    CShinyBrick(float x, float y, int type) : CQuestionBrick(x, y, type, 1) {}
    
    // Override methods to implement specific behavior

    //void OnCollisionWith(LPCOLLISIONEVENT e);
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    void Render();
    void SetState(int state);
    void Activate();
};
