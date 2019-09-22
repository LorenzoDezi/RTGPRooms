#include "ToonScene.h"

//TODO: Change shaders, skybox properly
ToonScene::ToonScene(Physics & simulation, Model & roomModel, std::vector<std::shared_ptr<Door>>& doors, float screenWidth, float screenHeight)
	: shader("Shaders/vertex_toon.glsl", "Shaders/fragment_toon.glsl"), 
	doorShader("Shaders/vertex_phong.glsl", "Shaders/fragment_door.glsl"), 
	skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl"), 
	faces { 
		"assets/textures/purplenebula_lf.tga",
		"assets/textures/purplenebula_rt.tga",
		"assets/textures/purplenebula_up.tga",
		"assets/textures/purplenebula_dn.tga",
		"assets/textures/purplenebula_ft.tga",
		"assets/textures/purplenebula_bk.tga" 
	},
	screenHeight(screenHeight),
	screenWidth(screenWidth),
	skybox(faces), doors(doors)
{
	this->roomModel = std::make_shared<Model>(roomModel);
	this->mainModel = std::make_shared<Model>("Assets/Gundam.obj");
	std::vector<std::shared_ptr<Shader>> shaders{
		std::make_shared<Shader>(shader),
	};
	model = std::make_shared<BlinnPhongModel>(shaders);
}

void ToonScene::Draw(Camera & camera, float time)
{
	//TODO: completely rearrange when you decide what to do
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();
	glm::mat4 projection;
	projection = glm::perspective(camera.Zoom, screenWidth / screenHeight, 0.1f, 100.0f);
	shader.use();
	shader.setMat4Float("view", glm::value_ptr(view));
	shader.setMat4Float("projection", glm::value_ptr(projection));
	shader.setVec3Float("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
	
	model->setLightParameters(glm::vec3(3.0f, 3.0f, 3.0f),
		glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	model->setMaterial(0.2f, 0.4f, 0.0f, 0.2f);

	//Doors rendering
	doorShader.use();
	doorShader.setMat4Float("view", glm::value_ptr(view));
	doorShader.setMat4Float("projection", glm::value_ptr(projection));
	doorShader.setFloat("time", time);
	for (auto& door : doors) {
		door->Draw(doorShader);
	}

	//room rendering
	model->setMaterial(0.3f, 0.8f, 0.1f, 0.2f);
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	shader.setFloat("textureScale", 3.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setMat4Float("model", glm::value_ptr(model));
	//DEBUG
	shader.setVec3Float("objectColor", 0.0, 1.0, 0.0);
	roomModel->Draw(shader);

	model = glm::mat4(1.0f);
	shader.setFloat("textureScale", 1.0f);
	model = glm::translate(model, glm::vec3(4.3f, 1.9f, 3.7f));
	shader.setMat4Float("model", glm::value_ptr(model));
	//DEBUG
	shader.setVec3Float("objectColor", 1.0, 0.0, 0.0);
	mainModel->Draw(shader);

	//skybox rendering
	skybox.Draw(skyboxShader, view, projection);
}

bool ToonScene::hasBloom()
{
	return false;
}

ToonScene::~ToonScene()
{
}
