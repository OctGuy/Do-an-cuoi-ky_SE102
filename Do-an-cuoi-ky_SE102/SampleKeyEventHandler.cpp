#include "SampleKeyEventHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"

CGame* game = CGame::GetInstance();

void CSampleKeyHandler::OnKeyDown(int KeyCode)
{
    CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

    // Always process pause key regardless of game state
    if (KeyCode == DIK_P)
    {
        game->TogglePause();
        return;
    }

    // If game is paused, ignore all other input
    if (game->IsPaused() 
        || game->IsTimeFrozen() 
        || mario->IsTunneling()
        || mario->IsInputBlocked())
        return;

    //DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
    switch (KeyCode)
    {
    case DIK_DOWN:
        mario->SetState(MARIO_STATE_SIT);
		mario->SetAbleToTunnelDown(true);
        break;
    case DIK_UP:
        mario->SetAbleToTunnelUp(true);
		break;
    case DIK_S:
        if (mario->GetLevel() == MARIO_LEVEL_RACCOON)
        {
            if (mario->IsOnPlatform()) // If on ground, do a normal jump
            {
                mario->SetState(MARIO_STATE_JUMP);
                if (mario->IsAbleToFly())
					mario->StartFlying();
            }
            else // If already in air
            {
                if (mario->IsAbleToFly())
                    mario->SetState(MARIO_STATE_FLYING);
                else
                    mario->SetState(MARIO_STATE_SLOW_FALL);
            }
        }
        else // For Small and Big Mario, just jump normally
        {
            mario->SetState(MARIO_STATE_JUMP);
        }
        break;
    case DIK_A:
        if (mario->GetLevel() == MARIO_LEVEL_RACCOON && !mario->IsTailAttacking() && !mario->IsSitting())
            mario->SetState(MARIO_STATE_TAIL_ATTACK);
		mario->SetState(MARIO_STATE_HOLD);
        break;
    case DIK_1:
        mario->SetLevel(MARIO_LEVEL_SMALL);
        break;
    case DIK_2:
        mario->SetLevel(MARIO_LEVEL_BIG);
        break;
	case DIK_3:
		mario->SetLevel(MARIO_LEVEL_RACCOON);
		break;
    case DIK_R: //reload scene
        game->ReloadCurrentScene();
        break;
    }
}

void CSampleKeyHandler::OnKeyUp(int KeyCode)
{
    CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
    CGame* game = CGame::GetInstance();

    // If game is paused, ignore all other input
    if (game->IsPaused()
        || game->IsTimeFrozen()
        || mario->IsTunneling()
        || mario->IsInputBlocked())
        return;

    //DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
    switch (KeyCode)
    {
    case DIK_RIGHT:
        //The order of these codditions is important
        if (mario->GetVx() > 0.01f)  // Only decelerate if moving at a meaningful speed
            mario->SetState(MARIO_STATE_DECELERATE_RIGHT);
        else
            mario->SetState(MARIO_STATE_IDLE);  // Stop immediately if moving very slowly
        // Check if down key is pressed when right key is released
        if (game->IsKeyDown(DIK_DOWN)) {
            mario->SetState(MARIO_STATE_SIT);
        }
        break;

    case DIK_LEFT:
        if (mario->GetVx() < -0.01f)  // Only decelerate if moving at a meaningful speed
            mario->SetState(MARIO_STATE_DECELERATE_LEFT);
        else
            mario->SetState(MARIO_STATE_IDLE);  // Stop immediately if moving very slowly
        if (game->IsKeyDown(DIK_DOWN))
            mario->SetState(MARIO_STATE_SIT);
        break;
    case DIK_A:
        mario->SetState(MARIO_STATE_DROP);
        break;
    case DIK_S:
        mario->SetState(MARIO_STATE_RELEASE_JUMP);
        // Check if down key is presed when jump key is released
        if (game->IsKeyDown(DIK_DOWN))
            mario->SetState(MARIO_STATE_SIT);
        break;
    case DIK_DOWN:
        mario->SetState(MARIO_STATE_SIT_RELEASE);
        mario->SetAbleToTunnelDown(false);
        break;
    case DIK_UP:
        mario->SetAbleToTunnelUp(false);
        break;
    }
}

void CSampleKeyHandler::KeyState(BYTE* states)
{
    CGame* game = CGame::GetInstance();
    CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

    // If game is paused, ignore all other input
    if (game->IsPaused()
        || game->IsTimeFrozen()
        || mario->IsTunneling()
        || mario->IsInputBlocked())
        return;

    if (game->IsKeyDown(DIK_S))
		mario->SetState(MARIO_STATE_JUMP);

    if (game->IsKeyDown(DIK_LEFT))
    {
        if (game->IsKeyDown(DIK_A))
            mario->SetState(MARIO_STATE_RUNNING_LEFT);
        else
            mario->SetState(MARIO_STATE_WALKING_LEFT);
    }
    else if (game->IsKeyDown(DIK_RIGHT))
    {
        if (game->IsKeyDown(DIK_A))
            mario->SetState(MARIO_STATE_RUNNING_RIGHT);
        else
            mario->SetState(MARIO_STATE_WALKING_RIGHT);
    }
    else
    {
        // Check velocity thresholds to determine when to stop
        float vx = mario->GetVx();

        // If Mario is in a deceleration state, let it continue
        if (mario->GetState() == MARIO_STATE_DECELERATE_RIGHT ||
            mario->GetState() == MARIO_STATE_DECELERATE_LEFT)
        {
            // If nearly stopped, set to IDLE
            if (abs(vx) < 0.01f)
                mario->SetState(MARIO_STATE_IDLE);
        }
        // If not already in a special state and not moving, go IDLE
        else if (abs(vx) < 0.01f)
            mario->SetState(MARIO_STATE_IDLE);
    }
}
