#include "CorridorScene.h"

//TODO: toggle simulation
CorridorScene::CorridorScene(Physics &simulation, Model &roomModel, std::vector<std::shared_ptr<Door>> &doors) :
	shader("Shaders/vertex_phong.glsl", "Shaders/fragment_phong.glsl"),
	doorShader("Shaders/vertex_door.glsl", "Shaders/fragment_door.glsl"),
	shaderLight("Shaders/vertex_lamp.glsl", "Shaders/fragment_lamp.glsl"),
	skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl"),
	roomModel(roomModel), torchModel("Assets/torch2.obj"),
	doorModel("Assets/door.obj"), sphereModel("Assets/sphere.obj"), faces{
	"assets/textures/purplenebula_lf.tga",
	"assets/textures/purplenebula_rt.tga",
	"assets/textures/purplenebula_up.tga",
	"assets/textures/purplenebula_dn.tga",
	"assets/textures/purplenebula_ft.tga",
	"assets/textures/purplenebula_bk.tga"}, 
	skybox(faces),
	lightSupportRotations { 0.f, 0.f, 0.f, 0.f, 135.f, 135.f },
	lightSupportPositions{
		glm::vec3(1.25f, 2.0f, 2.75f),
		glm::vec3(1.25f, 2.0f, 4.90f),
		glm::vec3(1.25f, 2.0f, -4.90f),
		glm::vec3(1.25f, 2.0f, -2.75f),
		glm::vec3(-1.25f, 2.0f, -1.0f),
		glm::vec3(-1.25f, 2.0f, 1.0f) },
	pointLightPositions{
		glm::vec3(1.60f, 2.75f, 2.75f),
		glm::vec3(1.60f, 2.75f, 4.90f),
		glm::vec3(1.60f, 2.75f, -4.90f),
		glm::vec3(1.60f, 2.75f, -2.75f),
		glm::vec3(-1.60f, 2.75f, -1.0f),
		glm::vec3(-1.60f, 2.75f, 1.0f) },
	doors(doors), 
	lightDir(0.0f, -1.0f, 0.0f), model(shader) {

	
}

void CorridorScene::Draw(Camera &camera, float time)
{
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();
	glm::mat4 projection;
	projection = glm::perspective(camera.Zoom, 1280.0f / 720.0f, 0.1f, 100.0f);
	shader.use();
	shader.setMat4Float("view", glm::value_ptr(view));
	shader.setMat4Float("projection", glm::value_ptr(projection));
	shader.setVec3Float("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
	model.setLightParameters(
		glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.02f, 0.02f, 0.02f));
	model.setDirLight(lightDir);

	//lights rendering
	shaderLight.use();
	shaderLight.setMat4Float("view", glm::value_ptr(view));
	shaderLight.setMat4Float("projection", glm::value_ptr(projection));
	model.setLightParameters(glm::vec3(3.0f, 3.0f, 3.0f),
		glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	for (int i = 0; i < NR_CORRIDOR_POINT_LIGHTS; i++) {
		model.setPointLight(pointLightPositions[i], i);
		shaderLight.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightSupportPositions[i] + glm::vec3(0.0f, 0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2));
		shaderLight.setMat4Float("model", glm::value_ptr(model));
		sphereModel.Draw(shaderLight);
	}
	//light supports rendering
	model.setMaterial(0.2f, 0.4f, 0.0f, 0.2f);
	for (int i = 0; i < NR_CORRIDOR_POINT_LIGHTS; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightSupportPositions[i]);
		model = glm::rotate(model, lightSupportRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));
		shader.use();
		shader.setMat4Float("model", glm::value_ptr(model));
		torchModel.Draw(shader);
	}

	//Doors rendering
	doorShader.use();
	doorShader.setMat4Float("view", glm::value_ptr(view));
	doorShader.setMat4Float("projection", glm::value_ptr(projection));
	doorShader.setFloat("time", time);
	for (auto& door : doors) {
		door->Draw(doorShader);
	}
	
	//room rendering
	model.setMaterial(0.3f, 0.8f, 0.1f, 0.2f);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	shader.use();
	shader.setMat4Float("model", glm::value_ptr(model));
	roomModel.Draw(shader);

	//skybox rendering
	skybox.Draw(skyboxShader, view, projection);
}

bool CorridorScene::hasBloom()
{
	return true;
}


CorridorScene::~CorridorScene()
{
}
