#include "ToonScene.h"

//TODO: Change shaders, skybox properly
ToonScene::ToonScene(Physics & simulation, Model & roomModel, std::vector<std::shared_ptr<Door>>& doors, float screenWidth, float screenHeight)
	: shader("Shaders/vertex_toon.glsl", "Shaders/fragment_toon.glsl"),
	outlineShader("Shaders/vertex_outline.glsl", "Shaders/fragment_outline.glsl"),
	doorShader("Shaders/vertex_phong.glsl", "Shaders/fragment_door.glsl"), 
	skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl"), 
	faces { 
		"assets/textures/craterlake_lf.tga",
		"assets/textures/craterlake_rt.tga",
		"assets/textures/craterlake_up.tga",
		"assets/textures/craterlake_dn.tga",
		"assets/textures/craterlake_ft.tga",
		"assets/textures/craterlake_bk.tga"
	},
	screenHeight(screenHeight),
	screenWidth(screenWidth),
	skybox(faces), doors(doors)
{
	this->roomModel = std::make_shared<Model>(roomModel);
	this->mainModel = std::make_shared<Model>("Assets/Gundam.obj");
	std::vector<std::shared_ptr<Shader>> shaders {
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
	outlineShader.use();
	outlineShader.setMat4Float("view", glm::value_ptr(view));
	outlineShader.setMat4Float("projection", glm::value_ptr(projection));
	this->model->setPointLight(glm::vec3(4.3f, 2.5f, 3.7f), 0);
	this->model->setLightParameters(glm::vec3(3.0f, 3.0f, 3.0f),
		glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	this->model->setMaterial(0.2f, 0.4f, 0.0f, 0.2f);

	//Doors rendering
	doorShader.use();
	doorShader.setMat4Float("view", glm::value_ptr(view));
	doorShader.setMat4Float("projection", glm::value_ptr(projection));
	doorShader.setFloat("time", time);
	for (auto& door : doors) {
		if(door->getSceneType() == TOON)
			door->Draw(doorShader);
	}
	//skybox rendering
	skybox.Draw(skyboxShader, view, projection);
	//Gundam rendering
	shader.use();
	this->model->setMaterial(0.2f, 0.8f, 1.0f, 0.8f);
	glm::mat4 model = glm::mat4(1.0f);
	shader.setFloat("textureScale", 1.0f);
	model = glm::translate(model, glm::vec3(4.3f, 1.9f, 3.7f));
	shader.setMat4Float("model", glm::value_ptr(model));
	//DEBUG
	glEnable(GL_STENCIL_TEST);
	//We replace the stencil value if both depth and stencil test succeeds
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF); // each bit is written to the stencil buffer as is
	glStencilFunc(GL_ALWAYS, 1, 0xFF); //Each fragment is written to the stencil buffer
	mainModel->Draw(shader);
	//Gundam outline rendering
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //Drawing only parts of the model that are not inside the previous one
	glStencilMask(0x00); // disable writing to the stencil buffer
	glDisable(GL_DEPTH_TEST); //disable depth testing, so borders always get rendered
	outlineShader.use();
	outlineShader.setMat4Float("model", glm::value_ptr(model));
	mainModel->Draw(outlineShader);
	glEnable(GL_DEPTH_TEST);
	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);
}

bool ToonScene::hasBloom()
{
	return false;
}

ToonScene::~ToonScene()
{
}
