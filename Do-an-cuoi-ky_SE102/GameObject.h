#pragma once

#include <Windows.h>
#include <d3dx10.h>
#include <vector>

#include "Animation.h"
#include "Animations.h"
#include "Sprites.h"
#include "Collision.h"

#include "debug.h"

using namespace std;

#define ID_TEX_BBOX -100		// special texture to draw object bounding box
#define BBOX_ALPHA 0.25f		// Bounding box transparency

class CGameObject
{
protected:

	float x; 
	float y;
	float loadX;	//the coordinates of the object when it is first loaded
	float loadY;

	float vx;
	float vy;

	int nx;	 

	int state;

	bool isDeleted; 

	bool isActive; // For loading item in question brick

public: 
	void SetPosition(float x, float y) { this->x = x, this->y = y; }
	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }
	void GetPosition(float &x, float &y) { x = this->x; y = this->y; }
	void GetLoadPosition(float& x, float& y) { x = this->loadX; y = this->loadY; }
	void GetSpeed(float &vx, float &vy) { vx = this->vx; vy = this->vy; }
	void GetNx(int& nx) { nx = this->nx; }
	void SetDirection(int direction) { nx = direction; }

	virtual void SetActive(bool isActive) { this->isActive = isActive;} 
	bool IsActive() { return isActive; }

	int GetState() { return this->state; }
	virtual void Delete() { isDeleted = true;  }
	bool IsDeleted() { return isDeleted; }

	virtual void RenderBoundingBox();

	CGameObject();
	CGameObject(float x, float y) :CGameObject() 
	{ 
		this->x = x; 
		this->loadX = x;
		this->y = y; 
		this->loadY = y;
	}


	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom) = 0;
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = nullptr) {};
	virtual void Render() = 0;
	virtual void SetState(int state) { this->state = state; }
	virtual void Reload();

	//
	// Collision ON or OFF ? This can change depending on object's state. For example: die
	//
	virtual int IsCollidable() { return 0; }

	//
	// When an object is tangible it can be block by other object
	//
	virtual int IsTangible() { return 1; } 

	// When no collision has been detected (triggered by CCollision::Process)
	virtual void OnNoCollision(DWORD dt) {};

	// When collision with an object has been detected (triggered by CCollision::Process)
	virtual void OnCollisionWith(LPCOLLISIONEVENT e) {};
	
	// Is this object blocking other object? If YES, collision framework will automatically push the other object
	virtual int IsBlocking() { return 1; }

	// Does this object collide with other object at certain direction ( like ColorBox )
	virtual int IsDirectionColliable(float nx, float ny) { return 1; }

	~CGameObject();

	static bool IsDeleted(const LPGAMEOBJECT &o) { return o->isDeleted; }
};
