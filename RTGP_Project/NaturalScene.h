#pragma once
#include "Shader.h"
#include "Model.h"
#include "Grass.h"
#include "Door.h"
#include "Camera.h"
#include "Scene.h"
#include "glm/glm.hpp"
#include "Physics.h"
#include "Skybox.h"
#include "BlinnPhongModel.h"

#define NR_NATURAL_POINT_LIGHTS 6

//TODO: Implements the class - now it's a copy of corridor scene

class NaturalScene : public Scene
{
public:
	NaturalScene(Physics &simulation, Model &roomModel, std::vector<std::shared_ptr<Door>> &doors);
	void Draw(Camera &camera, float time);
	bool hasBloom();
	~NaturalScene();

private:
	//Shaders
	Shader shader;
	Shader shaderLight;
	Shader leavesShader;
	//TODO
	//Shader shaderGrass;
	Shader doorShader;
	Shader skyboxShader;

	std::shared_ptr<BlinnPhongModel> model;

	//Models
	Model roomModel;
	Model doorModel;
	Model treeTrunkModel;
	Model treeLeavesModel;

	//Light positions setup
	glm::vec3 lightSupportPositions[NR_NATURAL_POINT_LIGHTS];
	glm::vec3 pointLightPositions[NR_NATURAL_POINT_LIGHTS];
	float lightSupportRotations[NR_NATURAL_POINT_LIGHTS];
	std::vector<std::shared_ptr<Door>> doors;
	glm::vec3 lightDir;

	//Grass setup
	Grass grass;
	std::vector<glm::vec3> grassPos;

	//Skybox setup
	std::string faces[6];
	Skybox skybox;

};

