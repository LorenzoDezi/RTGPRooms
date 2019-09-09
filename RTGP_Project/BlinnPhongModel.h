#pragma once
#include "LightingModel.h"
#include <vector>

class BlinnPhongModel :
	public LightingModel
{
public:
	BlinnPhongModel(std::shared_ptr<Shader> shader) : 
		lightingShaders{ shader }, ambient(0.0f), diffuse(0.0f), specular(0.0f) {};
	BlinnPhongModel(std::vector<std::shared_ptr<Shader>> &shaders) :
		lightingShaders(shaders), ambient(0.0f), diffuse(0.0f), specular(0.0f) {};
	void setLightParameters(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void setPointLight(glm::vec3 position, int i);
	void setDirLight(glm::vec3 direction);
	void setMaterial(float Ka, float Kd, float Ks, float shininess);
private:
	std::vector<std::shared_ptr<Shader>> lightingShaders;
	glm::vec3 ambient, diffuse, specular;
};

