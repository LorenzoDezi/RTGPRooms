#include "BlinnPhongModel.h"

void BlinnPhongModel::setLightParameters(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

void BlinnPhongModel::setPointLight(glm::vec3 position, int i)
{
	lightingShader.use();
	std::string refLight = "pointLights[" + std::to_string(i) + "]";
	lightingShader.setVec3Float(refLight + ".position", position.x,
		position.y, position.z);
	lightingShader.setFloat(refLight + ".constant", 1.0f);
	lightingShader.setFloat(refLight + ".linear", 0.5f);
	lightingShader.setFloat(refLight + ".quadratic", 1.4f);
	lightingShader.setVec3Float(refLight + ".ambient", ambient.x, ambient.y, ambient.z);
	lightingShader.setVec3Float(refLight + ".diffuse", diffuse.x, diffuse.y, diffuse.z);
	lightingShader.setVec3Float(refLight + ".specular", specular.x, specular.y, specular.z);
}

void BlinnPhongModel::setDirLight(glm::vec3 direction)
{
	lightingShader.use();
	lightingShader.setVec3Float("dirLight.direction", direction.x, direction.y, direction.z);
	lightingShader.setVec3Float("dirLight.ambient", ambient.x, ambient.y, ambient.z);
	lightingShader.setVec3Float("dirLight.diffuse", diffuse.x, diffuse.y, diffuse.z);
	lightingShader.setVec3Float("dirLight.specular", specular.x, specular.y, specular.z);
}

void BlinnPhongModel::setMaterial(float Ka, float Kd, float Ks, float shininess)
{
	lightingShader.use();
	lightingShader.setFloat("material.Ka", Ka);
	lightingShader.setFloat("material.Kd", Kd);
	lightingShader.setFloat("material.Ks", Ks);
	lightingShader.setFloat("material.shininess", shininess);
}

