#include "NaturalScene.h"

NaturalScene::NaturalScene(Physics &simulation, Model &roomModel, std::vector<std::shared_ptr<Door>> &doors
	, float screenWidth, float screenHeight) :
	shader("Shaders/vertex_pbr.glsl", "Shaders/fragment_pbr.glsl"),
	depthShader("Shaders/vertex_depth.glsl", "Shaders/fragment_depth.glsl"),
	doorShader("Shaders/vertex_door.glsl", "Shaders/fragment_door.glsl"),
	shaderLight("Shaders/vertex_lamp.glsl", "Shaders/fragment_lamp.glsl"),
	skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl"),
	leavesShader("Shaders/vertex_leaves.glsl", "Shaders/fragment_leaves.glsl"),
	roomModel(std::make_shared<Model>(roomModel)), treeTrunkModel("Assets/TreeTrunk.obj"),
	treeLeavesModel("Assets/TreeLeaves.obj"), faces {
	"assets/textures/craterlake_lf.tga",
	"assets/textures/craterlake_rt.tga",
	"assets/textures/craterlake_up.tga",
	"assets/textures/craterlake_dn.tga",
	"assets/textures/craterlake_ft.tga",
	"assets/textures/craterlake_bk.tga" },
	skybox(faces),
	screenHeight(screenHeight),
	screenWidth(screenWidth),
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
	buildShadowMap();
	model->setLightSpaceMatrix(lightSpaceMatrix);
	grass.setDepthMap(depthMap);
	treeLeavesModel.setDepthMap(depthMap);
	treeTrunkModel.setDepthMap(depthMap);
	this->roomModel->setDepthMap(depthMap);
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
	roomModel->Draw(shader);

	//trees rendering
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.0f, 0.01f, 0.0f));
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

void NaturalScene::DrawSceneDepth()
{
	glm::mat4 model = glm::mat4(1.0f);
	depthShader.use();
	depthShader.setMat4Float("model", glm::value_ptr(model));
	depthShader.setMat4Float("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
	roomModel->Draw(depthShader);
	model = glm::translate(model, glm::vec3(-4.0f, 0.3f, 0.0f));
	depthShader.setMat4Float("model", glm::value_ptr(model));
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	treeLeavesModel.Draw(depthShader);
	treeTrunkModel.Draw(depthShader);
	glDisable(GL_CULL_FACE);
}

bool NaturalScene::hasBloom()
{
	return false;
}


NaturalScene::~NaturalScene()
{
	glDeleteFramebuffers(1, &shadowMapFBO);
	glDeleteTextures(1, &depthMap);
}

void NaturalScene::buildShadowMap()
{
	shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);
	//Texture building
	const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//TODO Render scene with proper shader
	float near_plane = 1.0f, far_plane = 15.0f;
	glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, 
		-10.0f, 10.0f, near_plane, far_plane);
	glm::vec3 lightPosition(-4.0f, 10.5f, -4.7f);
	glm::mat4 lightView = glm::lookAt(lightPosition,
		glm::vec3(-4.0f, 0.3f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
	DrawSceneDepth();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Resetting viewPort and buffers
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
