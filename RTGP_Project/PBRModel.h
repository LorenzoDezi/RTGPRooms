#pragma once
#include "LightingModel.h"
class PBRModel : public LightingModel
{
public:
	PBRModel(std::shared_ptr<Shader> shader) : LightingModel(shader), 
		color(1.0f), metallic(0.0f) {};
	PBRModel(std::vector<std::shared_ptr<Shader>> shaders) : LightingModel(shaders), 
		color(1.0f), metallic(0.0f) {};
	void setMetallic(float metallic);
	void setPointLight(glm::vec3 position, int i);
	void setLightColor(glm::vec3 color);
	void setDirLight(glm::vec3 direction);
	~PBRModel();

private:
	float metallic;
	glm::vec3 color;
};

