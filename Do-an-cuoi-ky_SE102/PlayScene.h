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
#include "PowerUp.h"
#include "Portal.h"
#include "Pipe.h"
#include "SceneryObject.h"
#include "HUD.h"
//#include "Koopas.h"

#define LOAD_CHUNK_WIDTH 32
#define LOAD_CHUNK_HEIGHT 128

class CPlayScene: public CScene
{
protected: 
	// A play scene has to have player, right? 
	LPGAMEOBJECT player;
	LPHUD HUD;

	vector<LPGAMEOBJECT> objects;

	D3DXCOLOR backgroundColor;
	float rightBoundary;
	float bottomBoundary;

	BOOLEAN isCameraFollowMarioY = false; //Keep track of whether the camera should follow Mario's Y position

	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);

	void _ParseSection_ASSETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_SETTINGS(string line);

	void LoadAssets(LPCWSTR assetFile);
	
public: 
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	LPGAMEOBJECT GetPlayer() { return player; }
	void GetBoundary(float& right, float& bottom) { right = rightBoundary; bottom = bottomBoundary; }

	void Add(LPGAMEOBJECT obj) { objects.push_back(obj); }

	void Clear();
	void PurgeDeletedObjects();

	// Set background color
	void SetBackgroundColor(float r, float g, float b, float a = 1.0f) { backgroundColor = D3DXCOLOR(r, g, b, a); }

	//Check if object is within load chunk
	int IsWithinLoadChunk(LPGAMEOBJECT obj); //1: in load chunk. -1: out of load chunk. 0: within camera's view
	static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef CPlayScene* LPPLAYSCENE;

