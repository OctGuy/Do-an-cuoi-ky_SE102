#pragma once

#include "GameObject.h"

#define PORTAL_BBOX_WIDTH 16
#define PORTAL_BBOX_HEIGHT 24

/*
	Object that triggers scene switching
*/
class CPortal : public CGameObject
{
	int scene_id;	// target scene to switch to 

public:
	CPortal(float x, float y, int scene_id) : CGameObject(x, y)
	{
		this->scene_id = scene_id;
	}
	virtual void Render();
	void GetBoundingBox(float &l, float &t, float &r, float &b);
	void Reload();

	void SetActive(bool isActive);

	void RenderBoundingBox(void);
	
	int GetSceneId() { return scene_id;  }
	int IsBlocking() { return 0; }
	int IsCollidable() { return 1; }
	int IsTangible() { return 0; }
};