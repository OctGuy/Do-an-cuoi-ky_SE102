#include "HUD.h"
#include "GoalRoulette.h"
CHUD::CHUD(CMario* player)
{
    // Load sprites for HUD elements
    CSprites* sprites = CSprites::GetInstance();

    this->player = player;
    this->score = 0;
    this->coin = 0;
    this->lives = 4; // Default 4 lives
    this->remainingTime = 300; // Default 300 seconds
    this->timer_start = GetTickCount64();

	this->base = sprites->Get(ID_TEX_HUD); 
	marioIconSprite = sprites->Get(100037); //100037 is the sprite ID for Mario icon
}

void CHUD::Update(DWORD dt)
{
    // Update coin count from player
    if (player) {
        this->coin = player->GetCoin();
		this->score = player->GetPoint();
        this->pMeter = player->GetPMeter()/100;
		this->lives = player->GetLive();
		this->cards = player->GetCards();
    }

    // Update time
    ULONGLONG now = GetTickCount64();
    if (now - timer_start > 1000) // Every second
    {
        if (remainingTime > 0)
            remainingTime--;
        timer_start = now;
    }
}

void CHUD::Render()
{
    CGame* game = CGame::GetInstance();
    float cam_x, cam_y;
    game->GetCamPos(cam_x, cam_y);

    // Calculate HUD position (fixed at top of screen regardless of camera)
    float hud_x = cam_x + game->GetBackBufferWidth() / 2;
    float hud_y = cam_y + game->GetBackBufferHeight() - 16; // 16 pixels from bottom

	base->Draw(hud_x, hud_y); // Draw base HUD background

	//NOTE: The addition and subtraction of these values are to make the text is in the right position
	//For this the HUD to render correctly, these values need to be adjusted manually
	//TODO: create a way to calculate the position of the text
    
    // Render Mario icon and live number
	marioIconSprite->Draw(hud_x - 105, hud_y + 5);
    RenderNumber(lives, hud_x - 77, hud_y + 6, 1);

    // Render score
    RenderNumber(score, hud_x - 61, hud_y + 6, 7);

    // Render coins
    RenderNumber(coin, hud_x + 19, hud_y - 2, 2);

    // Render time
    RenderNumber(remainingTime, hud_x + 11, hud_y + 6, 3);

    //Render world number
	RenderNumber(1, hud_x - 77, hud_y - 2, 1);

    //Render P-Meter
    CAnimations* animations = CAnimations::GetInstance();
    int aniId = ID_ANI_P_METER_MAX;
    CSprites* sprites = CSprites::GetInstance();
    LPSPRITE sprite = sprites->Get(100038);

    for (int i = 0; i < pMeter; i++)
    {
        sprite->Draw(hud_x - 62 + FONT_BBOX_WIDTH*i, hud_y - 3.f);
    }
    if (pMeter == 6)
    {
        animations->Get(aniId)->Render(hud_x - 65 + FONT_BBOX_WIDTH * 7, hud_y - 3.f);
    }

	//Render cards
	for (size_t i = 0; i < cards.size(); i++)
	{
		if (cards[i] == CARD_TYPE_MUSHROOM)
			sprite = sprites->Get(ID_SPRITE_CARD_MUSHROOM);
		else if (cards[i] == CARD_TYPE_PLANT)
			sprite = sprites->Get(ID_SPRITE_CARD_PLANT);
		else if (cards[i] == CARD_TYPE_STAR)
			sprite = sprites->Get(ID_SPRITE_CARD_STAR);
        sprite->Draw(hud_x + 55 + CARD_BBOX_WIDTH * i, hud_y + 1.f);
	}
}

void CHUD::RenderNumber(int number, float x, float y, int length)
{
    string str_num = to_string(number);
    // Pad with leading zeros if needed
    while (str_num.length() < length)
        str_num = "0" + str_num;

    // Ensure we don't exceed the specified length
    if (str_num.length() > length)
        str_num = str_num.substr(str_num.length() - length);

    RenderText(str_num, x, y);
}

void CHUD::RenderText(string text, float x, float y)
{
    CSprites* sprites = CSprites::GetInstance();

    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        int index = 0;

        if (c >= '0' && c <= '9')
            // Use sprite IDs 100001-100010 for digits 0-9
            index = c - '0' + 100001;
        else if (c >= 'A' && c <= 'Z')
            // Use sprite IDs 100011-100036 for letters A-Z
            index = c - 'A' + 100011;

        LPSPRITE sprite = sprites->Get(index);
        if (sprite)
            sprite->Draw(x + i * FONT_BBOX_WIDTH, y);
    }
}
