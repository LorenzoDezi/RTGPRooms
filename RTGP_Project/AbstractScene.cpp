#include "AbstractScene.h"



AbstractScene::AbstractScene(Physics & simulation, Model & roomModel, std::vector<std::shared_ptr<Door>>& doors) :
	shader("Shaders/vertex_abstract.glsl", "Shaders/fragment_abstract.glsl", nullptr, 
		"Shaders/tControl_abstract.glsl", "Shaders/tEvaluation_abstract.glsl"), 
	doorShader("Shaders/vertex_door.glsl", "Shaders/fragment_door.glsl"),
	roomModel(std::make_shared<Model>(roomModel)),
	doors(doors),
	skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl"),
	faces {
	"assets/textures/mercury_lf.tga",
	"assets/textures/mercury_rt.tga",
	"assets/textures/mercury_up.tga",
	"assets/textures/mercury_dn.tga",
	"assets/textures/mercury_ft.tga",
	"assets/textures/mercury_bk.tga"
	}, skybox(faces)
{
	glPatchParameteri(GL_PATCH_VERTICES, 3);
}

void AbstractScene::Draw(Camera & camera, float time)
{
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();
	glm::mat4 projection;
	projection = glm::perspective(camera.Zoom, 1280.0f / 720.0f, 0.1f, 100.0f);
	shader.use();
	shader.setMat4Float("view", glm::value_ptr(view));
	shader.setMat4Float("projection", glm::value_ptr(projection));
	shader.setVec3Float("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
	shader.setFloat("time", time);

	//Doors rendering
	doorShader.use();
	doorShader.setMat4Float("view", glm::value_ptr(view));
	doorShader.setMat4Float("projection", glm::value_ptr(projection));
	doorShader.setFloat("time", time);
	for (auto& door : doors) {
		door->Draw(doorShader);
	}

	//room rendering
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	shader.use();
	shader.setMat4Float("model", glm::value_ptr(model));
	//DEBUG
	roomModel->DrawTessellated(shader);

	//skybox rendering
	skybox.Draw(skyboxShader, view, projection);
}

bool AbstractScene::hasBloom()
{
	return true;
}

AbstractScene::~AbstractScene()
{
}
