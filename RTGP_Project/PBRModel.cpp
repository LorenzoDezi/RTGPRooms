#include "PBRModel.h"


void PBRModel::setMetallic(float metallic)
{
	this->metallic = metallic;
	for (auto& shader : lightingShaders) {
		shader->setFloat("material.metallic", metallic);
	}
}

void PBRModel::setPointLight(glm::vec3 position, int i)
{
	//Not implemented
}

void PBRModel::setLightColor(glm::vec3 color)
{
	this->color = color;
}

void PBRModel::setDirLight(glm::vec3 direction)
{
	for (auto& shader : lightingShaders) {
		shader->use();
		shader->setVec3Float("dirLight.direction", direction.x, direction.y, direction.z);
		shader->setVec3Float("dirLight.color", color.x, color.y, color.z);
	}
}

PBRModel::~PBRModel()
{
}
