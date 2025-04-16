#pragma once
#include "GameObject.h"
#include "Sprites.h"
#include "Textures.h"
#include "Game.h"
#include "Mario.h"
#include "AssetIDs.h"

// Define texture and sprite IDs for HUD elements
#define ID_TEX_HUD 100000
#define FONT_BBOX_WIDTH 8
#define FONT_BBOX_HEIGHT 8

class CHUD
{
private:
    CMario* player;
    int score;
    int coin;
    int remainingTime;
    int lives;
    
    // Timer for counting down
    ULONGLONG timer_start;
    
    // Sprites for HUD items
    LPSPRITE base;
    LPSPRITE marioIconSprite;
    
public:
    CHUD(CMario* player);
    void Update(DWORD dt);
    void Render();
    
    // Helper methods
    void RenderNumber(int number, float x, float y, int length);
    void RenderText(string text, float x, float y);
}; typedef CHUD* LPHUD;
