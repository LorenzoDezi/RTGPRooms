#include "NaturalScene.h"

NaturalScene::NaturalScene(Physics &simulation, Model &roomModel, std::vector<std::shared_ptr<Door>> &doors) :
	shader("Shaders/vertex_phong.glsl", "Shaders/fragment_phong.glsl"),
	doorShader("Shaders/vertex_door.glsl", "Shaders/fragment_door.glsl"),
	shaderLight("Shaders/vertex_lamp.glsl", "Shaders/fragment_lamp.glsl"),
	skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl"),
	roomModel(roomModel), treesModels{},
	doorModel("Assets/door.obj"), faces{
	"assets/textures/craterlake_lf.tga",
	"assets/textures/craterlake_rt.tga",
	"assets/textures/craterlake_up.tga",
	"assets/textures/craterlake_dn.tga",
	"assets/textures/craterlake_ft.tga",
	"assets/textures/craterlake_bk.tga" },
	skybox(faces),
	lightSupportRotations{ 0.f, 0.f, 0.f, 0.f, 135.f, 135.f },
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

	
	glm::vec3 startPos(-6.09046, 0.0, -6.64406);
	srand(time(NULL));
	for (float x = 0.0f; x <= 4.23906f; x += (rand() % 3 + 0)/10.f) {
		for (float z = 0.0f; z <= 13.34869; z += (rand() % 3 + 0) / 10.f) {
			grassPos.push_back(glm::vec3{ startPos.x + x, startPos.y, startPos.z + z });
		}
	}
	grass.setPositions(grassPos);
	treesModels.push_back(new Model("Assets/Tree01.obj"));
	treesModels.push_back(new Model("Assets/Tree02.obj"));
	treesModels.push_back(new Model("Assets/Tree03.obj"));
}

void NaturalScene::Draw(Camera &camera, float time)
{
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();
	glm::mat4 projection;
	projection = glm::perspective(camera.Zoom, 1280.0f / 720.0f, 0.1f, 100.0f);
	shader.use();
	shader.setMat4Float("view", glm::value_ptr(view));
	shader.setMat4Float("projection", glm::value_ptr(projection));
	shader.setVec3Float("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
	//TODO: better light setup
	model.setLightParameters(
		glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	model.setDirLight(lightDir);

	////lights rendering
	model.setLightParameters(glm::vec3(3.0f, 3.0f, 3.0f),
		glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	for (int i = 0; i < NR_NATURAL_POINT_LIGHTS; i++) {
		model.setPointLight(pointLightPositions[i], i);
		//TODO: natural lights setup
	}

	//Doors rendering
	doorShader.use();
	doorShader.setMat4Float("view", glm::value_ptr(view));
	doorShader.setMat4Float("projection", glm::value_ptr(projection));
	doorShader.setFloat("time", time);
	for (auto door : doors) {
		door->Draw(doorShader);
	}

	//Grass rendering
	grass.Draw(view, projection, time);
	

	//room rendering
	model.setMaterial(0.3f, 0.8f, 0.1f, 0.2f);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	shader.use();
	shader.setMat4Float("model", glm::value_ptr(model));
	roomModel.Draw(shader);

	//trees rendering
	model = glm::mat4(1.0f);
	shader.use();
	shader.setMat4Float("model", glm::value_ptr(model));
	for (auto& tree : treesModels) {
		tree->Draw(shader);
	}

	//skybox rendering
	skybox.Draw(skyboxShader, view, projection);
}

bool NaturalScene::hasBloom()
{
	return true;
}


NaturalScene::~NaturalScene()
{
}