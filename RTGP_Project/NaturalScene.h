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
#include "PBRModel.h"

class NaturalScene : public Scene
{
public:
	NaturalScene(Physics &simulation, Model &roomModel, 
		std::vector<std::shared_ptr<Door>> &doors, float screenWidth, float screenHeight);
	void Draw(Camera &camera, float time);
	void DrawSceneDepth();
	bool hasBloom();
	~NaturalScene();

private:
	void buildShadowMap();
	//depth map
	GLuint depthMap;
	GLuint shadowMapFBO;
	glm::mat4 lightSpaceMatrix;
	//Shaders
	Shader shader;
	Shader depthShader;
	Shader shaderLight;
	Shader leavesShader;
	Shader doorShader;
	Shader skyboxShader;

	std::shared_ptr<PBRModel> model;

	//Models
	std::shared_ptr<Model> roomModel;
	Model treeTrunkModel;
	Model treeLeavesModel;

	std::vector<std::shared_ptr<Door>> doors;

	//Grass setup
	Grass grass;
	std::vector<glm::vec3> grassPos;

	//Skybox setup
	std::string faces[6];
	Skybox skybox;

	//Screen width and height
	const float screenWidth;
	const float screenHeight;

};

