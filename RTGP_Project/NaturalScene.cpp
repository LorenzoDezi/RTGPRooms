#include "NaturalScene.h"

NaturalScene::NaturalScene(Physics &simulation, Model &roomModel, std::vector<std::shared_ptr<Door>> &doors) :
	shader("Shaders/vertex_phong.glsl", "Shaders/fragment_pbr.glsl"),
	doorShader("Shaders/vertex_door.glsl", "Shaders/fragment_door.glsl"),
	shaderLight("Shaders/vertex_lamp.glsl", "Shaders/fragment_lamp.glsl"),
	skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl"),
	leavesShader("Shaders/vertex_leaves.glsl", "Shaders/fragment_leaves.glsl"),
	roomModel(roomModel), treeTrunkModel("Assets/TreeTrunk.obj"),
	treeLeavesModel("Assets/TreeLeaves.obj"),
	doorModel("Assets/door.obj"), faces{
	"assets/textures/craterlake_lf.tga",
	"assets/textures/craterlake_rt.tga",
	"assets/textures/craterlake_up.tga",
	"assets/textures/craterlake_dn.tga",
	"assets/textures/craterlake_ft.tga",
	"assets/textures/craterlake_bk.tga" },
	skybox(faces),
	doors(doors)
{
	std::vector<std::shared_ptr<Shader>> shaders {
		std::make_shared<Shader>(shader),
			std::make_shared<Shader>(leavesShader),
			grass.getShader()
	};
	model = std::make_shared<PBRModel>(shaders);
	model->setMetallic(0.0f);
	glm::vec3 startPos(-6.09046, 0.0, -6.64406);
	srand(time(NULL));
	for (float x = 0.0f; x <= 4.23906f; x += (rand() % 3 + 0)/10.f) {
		for (float z = 0.0f; z <= 13.34869; z += (rand() % 3 + 0) / 10.f) {
			grassPos.push_back(glm::vec3{ startPos.x + x, startPos.y, startPos.z + z });
		}
	}
	grass.setPositions(grassPos);
	
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
	model->setLightColor(glm::vec3(8.5f, 8.5f, 8.5f));
	model->setDirLight(glm::vec3(0.3f, -0.5f, 0.5f));
	model->setLightColor(glm::vec3(3.5f, 3.5f, 3.5f));
	model->setDirLight(glm::vec3(-0.3f, -0.5f, -0.5f));


	//Doors rendering
	doorShader.use();
	doorShader.setMat4Float("view", glm::value_ptr(view));
	doorShader.setMat4Float("projection", glm::value_ptr(projection));
	doorShader.setFloat("time", time);
	for (auto door : doors) {
		door->Draw(doorShader);
	}

	//Grass rendering
	grass.Draw(camera, view, projection, time);
	
	//room rendering
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	shader.use();
	shader.setMat4Float("model", glm::value_ptr(model));
	shader.setFloat("textureScale", 4.0f);
	roomModel.Draw(shader);

	//trees rendering
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0f, 0.3f, 0.0f));
	leavesShader.use();
	leavesShader.setMat4Float("view", glm::value_ptr(view));
	leavesShader.setMat4Float("projection", glm::value_ptr(projection));
	leavesShader.setVec3Float("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
	leavesShader.setMat4Float("model", glm::value_ptr(model));
	leavesShader.setFloat("time", time);
	treeLeavesModel.Draw(leavesShader);
	shader.use();
	shader.setFloat("textureScale", 1.0f);
	shader.setMat4Float("model", glm::value_ptr(model));
	treeTrunkModel.Draw(shader);

	//skybox rendering
	skybox.Draw(skyboxShader, view, projection);
}

bool NaturalScene::hasBloom()
{
	return false;
}


NaturalScene::~NaturalScene()
{
}
