#pragma once
#include "Scene.h"
#include "Skybox.h"
#include "BlinnPhongModel.h"
#include "Door.h"

class ToonScene : public Scene
{
public:
	ToonScene(Physics &simulation, Model &roomModel, 
		std::vector<std::shared_ptr<Door>> &doors, float screenWidth, float screenHeight);
	void Draw(Camera &camera, float time);
	bool hasBloom();
	~ToonScene();

private:
	Shader shader;
	Shader outlineShader;
	Shader doorShader;
	Shader skyboxShader;
	std::shared_ptr<BlinnPhongModel> model;

	std::shared_ptr<Model> roomModel;
	std::shared_ptr<Model> mainModel;
	std::shared_ptr<Model> outlineModel;
	std::vector<std::shared_ptr<Door>> doors;

	std::string faces[6];
	Skybox skybox;

	const float screenWidth;
	const float screenHeight;
};

