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

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	player = NULL;
	key_handler = new CSampleKeyHandler(this);
	backgroundColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f); // Default to black
}

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_OBJECTS	2

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

			obj = new CMario(x, y);
			player = (CMario*)obj;

			DebugOut(L"[INFO] Player object has been created!\n");
			break;
		}

		case OBJECT_TYPE_GOOMBA: obj = new CGoomba(x, y); break;

		case OBJECT_TYPE_BRICK:
			type = atoi(tokens[3].c_str());
			obj = new CBrick(x, y, type);
			break;

		case OBJECT_TYPE_QUESTION_BRICK:
			type = atoi(tokens[3].c_str());
			obj = new CQuestionBrick(x, y, type);
			//WARNING: load item you want the quesiton brick to contain first then load the brick

			if (!objects.empty())
			{
				((CQuestionBrick*)obj)->SetItem(objects.back());
				objects.back()->SetActive(false);
			}

			break;

		case OBJECT_TYPE_COIN: obj = new CCoin(x, y); break;

		case OBJECT_TYPE_POWER_UP: obj = new CPowerUp(x, y); break;

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

			/*if (!objects.empty())
			{
				dynamic_cast<CPipe*>(obj)->SetItem(objects.back());
				objects.back()->SetActive(false);
			}*/
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

			//DebugOut(L"Loaded Tussock with spriteIDs:\n");
			//for (size_t i = 0; i < spriteIDs.size(); i++) {
			//	for (size_t j = 0; j < spriteIDs[i].size(); j++) {
			//		DebugOut(L"%d ", spriteIDs[i][j]);
			//	}
			//	DebugOut(L"\n"); // New line after each row
			//}
			/*CGame* game = CGame::GetInstance();
			DebugOut(L"BackBufferWidth: %d\n", game->GetBackBufferWidth());
			DebugOut(L"BackBufferHeight: %d\n", game->GetBackBufferHeight());*/

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
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }	

		//
		// data section
		//
		switch (section)
		{ 
			case SCENE_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
			case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}

	//Loading HUD
	if(player) HUD = new CHUD(dynamic_cast<CMario*>(player));

	f.close();

	DebugOut(L"[INFO] Done loading scene  %s\n", sceneFilePath);
}

void CPlayScene::Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	//I just added mario to collision list so mario would collide correctly with other objects
	//CGame* game = CGame::GetInstance();

	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 0; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (!objects[i]->IsActive()) continue;
		objects[i]->Update(dt, &coObjects);
	}

	if(player) HUD->Update(dt);

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	//if (player == NULL) return; 

	//// Update camera to follow mario
	//float cx, cy;
	//player->GetPosition(cx, cy);

	//// Get camera position
	//float camX, camY;
	//game->GetCamPos(camX, camY);

	//// Map size
	//float mapWidth = 2816.0f;
	//float mapHeight = 432.0f;

	//// Define margin boundaries    
	//float marginX = 80.0f; // Horizontal margin    
	//float marginY = 40.0f;  // Vertical margin  

	//// Only move camera if Mario pushes outside the margin
	//if (cx > camX + game->GetBackBufferWidth() - marginX)
	//	camX = cx - (game->GetBackBufferWidth() - marginX);
	//else if (cx < camX + marginX)
	//	camX = cx - marginX;

	//if (cy > camY + game->GetBackBufferHeight() - marginY)
	//	camY = cy - (game->GetBackBufferHeight() - marginY);
	//else if (cy < camY + marginY)
	//	camY = cy - marginY;

	//if (camX < -8)
	//	camX = -8;
	//if (camX > mapWidth - game->GetBackBufferWidth() - 8)
	//	camX = mapWidth - game->GetBackBufferWidth();
	//if (camY < 0)
	//	camY = 0;
	//if (camY > mapHeight - game->GetBackBufferHeight() - 9)
	//	camY = mapHeight - game->GetBackBufferHeight() - 9;

	//game->SetCamPos(camX, camY);

	//// Purged deleted objects
	//PurgeDeletedObjects();
	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	// Update camera to follow mario
	float cx, cy;
	player->GetPosition(cx, cy);

	CGame* game = CGame::GetInstance();
	cx -= game->GetBackBufferWidth() / 2;
	cy -= game->GetBackBufferHeight() / 2;

	if (cx < 0) cx = 0;

	CGame::GetInstance()->SetCamPos(cx, 220.f);

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