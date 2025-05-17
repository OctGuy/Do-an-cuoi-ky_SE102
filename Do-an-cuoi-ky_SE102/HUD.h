#pragma once
#include "GameObject.h"
#include "Sprites.h"
#include "Textures.h"
#include "Game.h"
#include "Mario.h"
#include "AssetIDs.h"

#define ID_ANI_P_METER_MAX 9000

// Define texture and sprite IDs for HUD elements
#define ID_TEX_HUD 100000
#define FONT_BBOX_WIDTH 8
#define FONT_BBOX_HEIGHT 8

#define CARD_BBOX_WIDTH 24
#define ID_SPRITE_CARD_MUSHROOM 100050
#define ID_SPRITE_CARD_PLANT 100051
#define ID_SPRITE_CARD_STAR 100052

class CHUD
{
private:
    CMario* player;
    int score;
    int coin;
    int pMeter; //Diferent from mario pMeter (0->6 instead of 0->600)
    int remainingTime;
    int lives;
	vector<int> cards;
    
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
