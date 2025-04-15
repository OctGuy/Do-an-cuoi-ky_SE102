#include "HUD.h"

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
    
    // Render Mario icon (for lives)
	marioIconSprite->Draw(hud_x - 105, hud_y + 5);
    RenderNumber(lives, hud_x - 85, hud_y + 6, 1);

    // Render score
    RenderNumber(score, hud_x - 61, hud_y + 6, 7);

    // Render coins
    RenderNumber(coin, hud_x + 19, hud_y - 2, 2);

    // Render time
    RenderNumber(remainingTime, hud_x + 11, hud_y + 6, 3);

    //Render world
	RenderNumber(1, hud_x - 77, hud_y - 2, 1);
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
