#pragma once
#include "Scene.h"
#include "Physics.h"
#include "Model.h"
#include "BezierSurface.h"
#include "ImageUtility.h"
#include "Camera.h"
#include "Door.h"
#include "Skybox.h"
#include <vector>

class AbstractScene :
	public Scene
{
public:
	AbstractScene(Physics &simulation, Model &roomModel, std::vector<std::shared_ptr<Door>> &doors,
		float screenWidth, float screenHeight);
	void Draw(Camera &camera, float time);
	bool hasBloom();
	~AbstractScene();

private:
	Shader shader;
	Shader doorShader;
	Shader faceShader;
	Shader skyboxShader;
	Shader bezierShader;

	GLuint displaceTexture;

	std::shared_ptr<Model> roomModel;
	std::vector<std::shared_ptr<Door>> doors;
	std::shared_ptr<BezierSurface> surface;

	std::string faces[6];
	Skybox skybox;

	const float screenWidth, screenHeight;
};

