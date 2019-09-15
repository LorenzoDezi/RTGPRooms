#pragma once
#include "Scene.h"
#include "Physics.h"
#include "Model.h"
#include "Camera.h"
#include "Door.h"
#include "Skybox.h"
#include <vector>

class AbstractScene :
	public Scene
{
public:
	AbstractScene(Physics &simulation, Model &roomModel, std::vector<std::shared_ptr<Door>> &doors);
	void Draw(Camera &camera, float time);
	bool hasBloom();
	~AbstractScene();

private:
	Shader shader;
	Shader doorShader;
	Shader skyboxShader;

	std::shared_ptr<Model> roomModel;
	std::vector<std::shared_ptr<Door>> doors;

	std::string faces[6];
	Skybox skybox;
};

