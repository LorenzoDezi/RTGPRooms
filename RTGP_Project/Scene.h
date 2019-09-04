#pragma once
#include "Camera.h"

class Scene {
	virtual void Draw(Camera camera, float time) = 0;
	virtual bool hasBloom() = 0;
};
