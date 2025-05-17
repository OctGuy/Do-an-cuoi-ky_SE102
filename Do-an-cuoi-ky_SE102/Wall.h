#pragma once

#include "Brick.h"

class CWall : public CBrick {
    float rightBound;
    float bottomBound;
public:

    CWall(float x, float y, float rightBound, float bottomBound,int type) : CBrick(x, y, type) 
    {
		this->rightBound = rightBound;
		this->bottomBound = bottomBound;
    }

    void Render() override {
        // Use the same rendering logic as CBrick because we wont be rendering it at all :D (am using this for invis wall)
        CBrick::Render();
		//RenderBoundingBox();
    }

    void GetBoundingBox(float& l, float& t, float& r, float& b) override {
        l = x - 8.f;
        t = y - 8.f;
        r = rightBound;
        b = bottomBound;
    }
};
