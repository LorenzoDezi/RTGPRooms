#pragma once
#include "LightingModel.h"

class BlinnPhongModel :
	public LightingModel
{
public:
	BlinnPhongModel(std::shared_ptr<Shader> shader) : 
		LightingModel(shader), ambient(0.0f), diffuse(0.0f), specular(0.0f) {};
	BlinnPhongModel(std::vector<std::shared_ptr<Shader>> &shaders) :
		LightingModel(shaders), ambient(0.0f), diffuse(0.0f), specular(0.0f) {};
	void setLightParameters(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void setPointLight(glm::vec3 position, int i);
	void setDirLight(glm::vec3 direction);
	void setMaterial(float Ka, float Kd, float Ks, float shininess);
private:
	glm::vec3 ambient, diffuse, specular;
};

