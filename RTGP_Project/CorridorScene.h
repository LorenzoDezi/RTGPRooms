#pragma once
#include "Shader.h"
#include "Model.h"
#include "Door.h"
#include "Camera.h"
#include "Scene.h"
#include "glm/glm.hpp"
#include "Physics.h"
#include "Skybox.h"
#include "BlinnPhongModel.h"

#define CORRIDOR_POINT_LIGHTS 6

class CorridorScene : Scene
{
public:
	CorridorScene(Physics &simulation, Model &roomModel, std::vector<Door> doors);
	void Draw(Camera &camera, float time);
	bool hasBloom();
	~CorridorScene();

private:
	//Shaders
	Shader shader;
	Shader shaderLight;
	Shader doorShader;
	Shader skyboxShader;
	BlinnPhongModel model;

	//Models
	Model roomModel;
	Model doorModel;
	Model torchModel;
	Model sphereModel;

	//Light positions setup
	glm::vec3 lightSupportPositions[CORRIDOR_POINT_LIGHTS];
	glm::vec3 pointLightPositions[CORRIDOR_POINT_LIGHTS];
	float lightSupportRotations[CORRIDOR_POINT_LIGHTS];
	std::vector<Door> doors;
	glm::vec3 lightDir;

	//Skybox setup
	std::string faces[6];
	Skybox skybox;

};

