#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "QuestionBrick.h"
#include "Mario.h"
#include "Goomba.h"
#include "BoxPlatform.h"
#include "Coin.h"
#include "Platform.h"
#include "Mushroom.h"
#include "Portal.h"
#include "Pipe.h"
//#include "Koopas.h"


class CPlayScene: public CScene
{
protected: 
	// A play scene has to have player, right? 
	LPGAMEOBJECT player;					

	vector<LPGAMEOBJECT> objects;

	D3DXCOLOR backgroundColor;

	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);

	void _ParseSection_ASSETS(string line);
	void _ParseSection_OBJECTS(string line);

	void LoadAssets(LPCWSTR assetFile);
	
public: 
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	LPGAMEOBJECT GetPlayer() { return player; }

	void Add(LPGAMEOBJECT obj) { objects.push_back(obj); }

	void Clear();
	void PurgeDeletedObjects();

	// Set background color
	void SetBackgroundColor(float r, float g, float b, float a = 1.0f) { backgroundColor = D3DXCOLOR(r, g, b, a); }

	static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef CPlayScene* LPPLAYSCENE;

