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

#define NR_CORRIDOR_POINT_LIGHTS 6

class CorridorScene : public Scene
{
public:
	CorridorScene(Physics &simulation, Model &roomModel, std::vector<std::shared_ptr<Door>> &doors);
	void Draw(Camera &camera, float time);
	bool hasBloom();
	~CorridorScene();

private:
	//Shaders
	Shader shader;
	Shader shaderLight;
	Shader doorShader;
	Shader torchShader;
	Shader skyboxShader;
	std::shared_ptr<BlinnPhongModel> model;

	//Models
	std::shared_ptr<Model> roomModel;
	Model torchModel;
	Model sphereModel;

	//Light positions setup
	glm::vec3 lightSupportPositions[NR_CORRIDOR_POINT_LIGHTS];
	glm::vec3 pointLightPositions[NR_CORRIDOR_POINT_LIGHTS];
	float lightSupportRotations[NR_CORRIDOR_POINT_LIGHTS];
	std::vector<std::shared_ptr<Door>> doors;
	glm::vec3 lightDir;

	//Skybox setup
	std::string faces[6];
	Skybox skybox;

};

