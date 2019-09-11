#pragma once
#include "Shader.h"
#include <vector>
#include "glm/glm.hpp"

class LightingModel {

public:
	LightingModel(std::vector<std::shared_ptr<Shader>> lightingShaders) : lightingShaders{ lightingShaders } {};
	LightingModel(std::shared_ptr<Shader> shader) : lightingShaders{ shader } {};
	virtual void setPointLight(glm::vec3 position, int i) = 0;
	virtual void setDirLight(glm::vec3 direction) = 0;

protected:
	std::vector<std::shared_ptr<Shader>> lightingShaders;
	

};
