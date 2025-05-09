#include <iostream>
#include <fstream>
#include "AssetIDs.h"

#include "PlayScene.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Coin.h"
#include "Platform.h"
#include "SceneryObject.h"
#include "SampleKeyEventHandler.h"
#include "PiranhaPlant.h"
#include "FireBullet.h"
#include "Koopa.h"
#include "RaccoonTail.h"
#include "Wall.h"
#include "ShinyBrick.h"
#include "PSwitch.h"
#include "WingedGoomba.h"

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	player = NULL;
	key_handler = new CSampleKeyHandler(this);
	backgroundColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f); // Default to black
	rightBoundary = 0;
	bottomBoundary = 0;
}

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_OBJECTS	2
#define SCENE_SECTION_SETTINGS	3

#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2

#define MAX_SCENE_LINE 1024

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPTEXTURE tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return; 
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CPlayScene::_ParseSection_ASSETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;

	wstring path = ToWSTR(tokens[0]);
	
	LoadAssets(path.c_str());
}

void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i+1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

/*
	Parse a line in section [OBJECTS] 
*/
void CPlayScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	// skip invalid lines - an object set must have at least id, x, y
	if (tokens.size() < 2) return;

	int object_type = atoi(tokens[0].c_str());
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	CGameObject *obj = NULL;

	int type = 0;

	switch (object_type)
	{
		case OBJECT_TYPE_MARIO:
		{
			if (player != NULL)
			{
				DebugOut(L"[ERROR] MARIO object was created before!\n");
				return;
			}

			// Create Mario
			obj = new CMario(x, y);
			player = (CMario*)obj;

			// Create the tail for Mario
			LPGAMEOBJECT tail = new CRaccoonTail(x, y + 16.f);

			// Set the tail for Mario
			dynamic_cast<CMario*>(player)->SetTail(tail);

			// Add the tail to the object list
			objects.push_back(tail);

			DebugOut(L"[INFO] Player object has been created!\n");
			break;
		}

		case OBJECT_TYPE_GOOMBA: obj = new CGoomba(x, y); break;

		case OBJECT_TYPE_BRICK:
			type = atoi(tokens[3].c_str());
			obj = new CBrick(x, y, type);
			break;

		case OBJECT_TYPE_WALL:
		{
			float rightBound = (float)atof(tokens[3].c_str());
			float bottomBound = (float)atof(tokens[4].c_str());
			type = atoi(tokens[5].c_str());
			obj = new CWall(x, y, rightBound, bottomBound, type);
			break;
		}

		case OBJECT_TYPE_QUESTION_BRICK:
		{
			type = atoi(tokens[3].c_str());
			int itemType = atoi(tokens[4].c_str());
			obj = new CQuestionBrick(x, y, type, itemType);
			break;
		}

		case OBJECT_TYPE_SHINY_BRICK:
			type = atoi(tokens[3].c_str());
			obj = new CShinyBrick(x, y, type);
			break;

		case OBJECT_TYPE_COIN: obj = new CCoin(x, y); break;

		case OBJECT_TYPE_POWER_UP: obj = new CPowerUp(x, y); break;

		case OBJECT_TYPE_PSWTICH:
		{
			obj = new CPSwitch(x, y);
			break;
		}

		case OBJECT_TYPE_PLATFORM:
		{
			float cell_width = (float)atof(tokens[3].c_str());
			float cell_height = (float)atof(tokens[4].c_str());
			int length = atoi(tokens[5].c_str());
			int sprite_begin = atoi(tokens[6].c_str());
			int sprite_middle = atoi(tokens[7].c_str());
			int sprite_end = atoi(tokens[8].c_str());

			obj = new CPlatform(
				x, y,
				cell_width, cell_height, length,
				sprite_begin, sprite_middle, sprite_end
			);

			break;
		}

		case OBJECT_TYPE_BOX_PLATFORM:
		{
			float cell_width = (float)atof(tokens[3].c_str());
			float cell_height = (float)atof(tokens[4].c_str());
			int length = atoi(tokens[5].c_str());
			int width = atoi(tokens[6].c_str());
			int sprite_id_TL = atoi(tokens[7].c_str());
			int sprite_id_MT = atoi(tokens[8].c_str());
			int sprite_id_TR = atoi(tokens[9].c_str());
			int sprite_id_ML = atoi(tokens[10].c_str());
			int sprite_id_fill = atoi(tokens[11].c_str());
			int sprite_id_MR = atoi(tokens[12].c_str());
			int sprite_id_BL = atoi(tokens[13].c_str());
			int sprite_id_MB = atoi(tokens[14].c_str());
			int sprite_id_BR = atoi(tokens[15].c_str());
			int sprite_id_SOTCorner = atoi(tokens[16].c_str());
			int sprite_id_SOTBody = atoi(tokens[17].c_str());
			int sprite_id_SOTBottom = atoi(tokens[18].c_str());

			obj = new CBoxPlatform(
				x, y, 
				length, width, cell_width, cell_height,
				sprite_id_TL, sprite_id_TR, sprite_id_BL, sprite_id_BR, sprite_id_fill,
				sprite_id_MT, sprite_id_MB, sprite_id_ML, sprite_id_MR, 
				sprite_id_SOTCorner, sprite_id_SOTBody, sprite_id_SOTBottom
			);

			break;
		}

		case OBJECT_TYPE_PIPE:
		{
			float cell_width = (float)atof(tokens[3].c_str());
			float cell_height = (float)atof(tokens[4].c_str());
			int height = atoi(tokens[5].c_str());
			int sprite_tl = atoi(tokens[6].c_str());
			int sprite_tr = atoi(tokens[7].c_str());
			int sprite_bl = atoi(tokens[8].c_str());
			int sprite_br = atoi(tokens[9].c_str());

			obj = new CPipe(
				x, y,
				cell_width, cell_height, height,
				sprite_tl, sprite_tr, sprite_bl, sprite_br
			);
			break;
		}

		case OBJECT_TYPE_SCENERY_OBJ: 
		{
			float cellWidth = (float)atof(tokens[3].c_str());
			float cellHeight = (float)atof(tokens[4].c_str());
			int length = atoi(tokens[5].c_str());
			int width = atoi(tokens[6].c_str());

			vector<vector<int>> spriteIDs;
			for (int i = 0; i < width; i++)
			{
				vector<int> row;
				for (int j = 0; j < length; j++)
				{
					int spriteID = atoi(tokens[7 + i * length + j].c_str());
					row.push_back(spriteID);
				}
				spriteIDs.push_back(row);
			}

			obj = new CSceneryObject(x, y, length, width, cellWidth, cellHeight, spriteIDs);
			break;
		}

		case OBJECT_TYPE_PIRANHA_PLANT:
		{
			obj = new CPiranhaPlant(x, y);
			break;
		}

		case OBJECT_TYPE_KOOPA:
		{
			obj = new CKoopa(x, y);
			break;
		}

		case OBJECT_TYPE_WINGED_GOOMBA:
		{
			obj = new CWingedGoomba(x, y);
			DebugOut(L"Loaded Winged Goomba\n");
			break;
		}

		case OBJECT_TYPE_PORTAL:
		{
			float r = (float)atof(tokens[3].c_str());
			float b = (float)atof(tokens[4].c_str());
			int scene_id = atoi(tokens[5].c_str());
			obj = new CPortal(x, y, r, b, scene_id);
		}
		break;

		default:
			DebugOut(L"[ERROR] Invalid object type: %d\n", object_type);
			return;
	}

	// General object setup
	obj->SetPosition(x, y);

	objects.push_back(obj);
}

void CPlayScene::_ParseSection_SETTINGS(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 2) return;
	if (tokens[0] == "right")
		rightBoundary = (float)atof(tokens[1].c_str());
	else if (tokens[0] == "bottom")
		bottomBoundary = (float)atof(tokens[1].c_str());
	else
		DebugOut(L"[ERROR] Unknown scene setting: %s\n", ToWSTR(tokens[0]).c_str());
}

void CPlayScene::LoadAssets(LPCWSTR assetFile)
{
	DebugOut(L"[INFO] Start loading assets from : %s \n", assetFile);

	ifstream f;
	f.open(assetFile);

	int section = ASSETS_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[SPRITES]") { section = ASSETS_SECTION_SPRITES; continue; };
		if (line == "[ANIMATIONS]") { section = ASSETS_SECTION_ANIMATIONS; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case ASSETS_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case ASSETS_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		}
	}

	f.close();

	DebugOut(L"[INFO] Done loading assets from %s\n", assetFile);
}

void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene from : %s \n", sceneFilePath);

	SetBackgroundColor(181.0f / 255.0f, 235.0f / 255.0f, 242.0f / 255.0f, 255.0f / 255.0f);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;					

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	
		if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; };
		if (line == "[OBJECTS]") { section = SCENE_SECTION_OBJECTS; continue; };
		if (line == "[SETTINGS]") { section = SCENE_SECTION_SETTINGS; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }	

		//
		// data section
		//
		switch (section)
		{ 
			case SCENE_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
			case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
			case SCENE_SECTION_SETTINGS: _ParseSection_SETTINGS(line); break;
		}
	}

	//Loading HUD
	if(player) HUD = new CHUD(dynamic_cast<CMario*>(player));

	f.close();

	// Initialize camera position to ensure Mario is loaded correctly on the first frame
	if (player) {
		float marioX, marioY;
		player->GetPosition(marioX, marioY);

		CGame* game = CGame::GetInstance();
		float camX, camY;

		// Center camera on Mario 
		camX = marioX - game->GetBackBufferWidth() / 2.0f;
		camY = marioY - game->GetBackBufferHeight() / 2.0f;

		float gameWidth = game->GetBackBufferWidth();

		if (camX < 0.0f) {
			camX = 0.0f;
		}
		else {
			if (rightBoundary > 0) {
				float upperXLimit = rightBoundary - gameWidth - 9.0f;

				if (upperXLimit < 0.0f) upperXLimit = 0.0f;

				if (camX > upperXLimit) {
					camX = upperXLimit;
				}
			}
		}

		if (camY < 0.0f) {
			camY = 0.0f;
		}
		else {

			if (bottomBoundary > 0) { 
				if (camY > bottomBoundary) {
					camY = bottomBoundary;
				}
			}
		}

		CGame::GetInstance()->SetCamPos(camX, camY);
	}
	DebugOut(L"[INFO] Done loading scene  %s\n", sceneFilePath);
}

void CPlayScene::Update(DWORD dt)
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i] == player)
			continue;

		if (IsWithinLoadChunk(objects[i]) == -1)
		{
			objects[i]->Reload();
			objects[i]->SetActive(false);
		}
		else if (IsWithinLoadChunk(objects[i]) == 1)
		{
			objects[i]->SetActive(true);
		}
	}

	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (!objects[i]->IsActive()) continue;
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (!objects[i]->IsActive()) continue;
		objects[i]->Update(dt, &coObjects);
	}

	if(player) HUD->Update(dt);

	if (player == NULL) return;
	// Update camera to follow mario
	float cx, cy;
	player->GetPosition(cx, cy);

	CGame* game = CGame::GetInstance();

	// Get Mario's vertical velocity to check if he's flying
	float vy = ((CMario*)player)->GetVy();
	CMario* mario = (CMario*)player;

	// Follow on X axis
	cx -= game->GetBackBufferWidth() / 2;

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	// If mario is flying then switch camera to follow him on y axis else keep it fixed
	if (mario->IsInAir() && vy < 0 || marioY < 200.f)
		isCameraFollowMarioY = true;
	else if (mario->IsOnPlatform() && marioY >= 340.f)
		isCameraFollowMarioY = false;

	//Make camera follow Mario vertically when flying
	if (isCameraFollowMarioY) {
		cy -= game->GetBackBufferHeight() / 2;
	}
	else {
		// Regular fixed camera Y position when not flying
		cy = 220.0f;
	}

	if (cx < 0) cx = 0;

	//Boundary is set in the scene file under SETTINGS section
	else if (cx > rightBoundary - game->GetBackBufferWidth() - 9.f) cx = rightBoundary - game->GetBackBufferWidth() - 9.f;

	if (cy < 0) cy = 0;
	else if (cy > bottomBoundary) cy = bottomBoundary;

	CGame::GetInstance()->SetCamPos(cx, cy);

	PurgeDeletedObjects();
}

void CPlayScene::Render()
{
	// Clear the back buffer with the background color
	CGame* game = CGame::GetInstance();
	ID3D10Device* pD3DDevice = game->GetDirect3DDevice();
	ID3D10RenderTargetView* pRTV = game->GetRenderTargetView();

	// Clear the back buffer with the background color
	pD3DDevice->ClearRenderTargetView(pRTV, backgroundColor);

	// Render all game objects
	for (int i = 0; i < objects.size(); i++) {
		if (!objects[i]->IsActive()) continue;
		objects[i]->Render();
	}

	// Render HUD
	if (player) HUD->Render();
}

/*
*	Clear all objects from this scene
*/
void CPlayScene::Clear()
{
	vector<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		delete (*it);
	}
	objects.clear();
}

/*
	Unload scene

	TODO: Beside objects, we need to clean up sprites, animations and textures as well 

*/
void CPlayScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();
	player = NULL;

	if (HUD) {
		delete HUD;
		HUD = nullptr;
	}

	DebugOut(L"[INFO] Scene %d unloaded! \n", id);
}

int CPlayScene::IsWithinLoadChunk(LPGAMEOBJECT obj)
{
	CGame* game = CGame::GetInstance();
	float sceneWidth = game->GetBackBufferWidth();
	float sceneHeight = game->GetBackBufferHeight();

	float leftBound, topBound, rightBound, bottomBound;
	obj->GetBoundingBox(leftBound, topBound, rightBound, bottomBound);
	
	float camx, camy;
	game->GetCamPos(camx, camy);

	//if ((rightBound >= camx && leftBound <= camx + sceneWidth) &&
	//	(bottomBound >= camy && topBound <= camy + sceneHeight))
	//	return TRUE;
	//else
	//	return FALSE;

	if (rightBound <= camx - LOAD_CHUNK_WIDTH || leftBound >= camx + sceneWidth + LOAD_CHUNK_WIDTH ||
		bottomBound <= camy - LOAD_CHUNK_HEIGHT || topBound >= camy + sceneHeight + LOAD_CHUNK_HEIGHT)
		return -1;
	else if (rightBound >= camx && leftBound <= camx + sceneWidth &&
			 bottomBound >= camy && topBound <= camy + sceneHeight)
		return 0;
	else
		return 1;
}

bool CPlayScene::IsGameObjectDeleted(const LPGAMEOBJECT& o) { return o == NULL; }

void CPlayScene::PurgeDeletedObjects()
{
	vector<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		LPGAMEOBJECT o = *it;
		if (o->IsDeleted())
		{
			delete o;
			*it = NULL;
		}
	}

	// NOTE: remove_if will swap all deleted items to the end of the vector
	// then simply trim the vector, this is much more efficient than deleting individual items
	objects.erase(
		std::remove_if(objects.begin(), objects.end(), CPlayScene::IsGameObjectDeleted),
		objects.end());
}