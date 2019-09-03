#pragma once
#include "Shader.h"
#include "glm/glm.hpp"

class LightingModel {

public:
	virtual void setPointLight(glm::vec3 position, int i) = 0;
	virtual void setDirLight(glm::vec3 direction) = 0;

};
