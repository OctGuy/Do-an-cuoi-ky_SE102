#include "1UpMushroom.h"

void C1UpMushroom::Render()
{
    CAnimations* animations = CAnimations::GetInstance();
    CSprites* sprites = CSprites::GetInstance();

    if (isRising)
    {
        // Play the rising animation
        animations->Get(ID_ANI_1UP_MUSHROOM_RISE)->Render(x, y - 3.f);
    }
    else
    {
        // Draw the regular sprite
        sprites->Get(ID_SPRITE_1UP_MUSHROOM)->Draw(x, y); 
    }
}
