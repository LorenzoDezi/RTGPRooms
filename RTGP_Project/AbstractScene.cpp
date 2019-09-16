#include "AbstractScene.h"



AbstractScene::AbstractScene(Physics & simulation, Model & roomModel, std::vector<std::shared_ptr<Door>>& doors) :
	shader("Shaders/vertex_abstract.glsl", "Shaders/fragment_abstract.glsl", nullptr, 
		"Shaders/tControl_abstract.glsl", "Shaders/tEvaluation_abstract.glsl"),
	faceShader("Shaders/vertex_abstract.glsl", "Shaders/fragment_face.glsl", nullptr,
		"Shaders/tControl_abstract.glsl", "Shaders/tEvaluation_face.glsl"),
	doorShader("Shaders/vertex_door.glsl", "Shaders/fragment_door.glsl"),
	bezierShader("Shaders/vertex_bezier.glsl", "Shaders/fragment_bezier.glsl", nullptr, 
		"Shaders/tControl_bezier.glsl", "Shaders/tEvaluation_bezier.glsl"),
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
}, displaceTexture(0), skybox(faces)
{
	//Bezier setup
	std::vector<glm::vec3> controlPoints;
	glm::vec3 controlPoint(0.0f);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			controlPoint.x = j * -0.5f;
			controlPoints.push_back(controlPoint);
		}
		controlPoint.z = i * 0.5f;
		controlPoint.x = 0.f;
	}
		
	std::vector<GLuint> controlPointIndices{
		0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6,
		4, 5, 9, 8, 5, 6, 10, 9, 6, 7, 11, 10,
		8, 9, 13, 12, 9, 10, 14, 13, 10, 11, 15, 14
	};
	surface = std::make_shared<BezierSurface>(controlPoints, controlPointIndices);
	displaceTexture = ImageUtility::TextureFromFile("DisplaceFace.png", "Assets/textures");
	//Adjusting texture parameters
	glBindTexture(GL_TEXTURE_2D, displaceTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

	bezierShader.use();
	bezierShader.setMat4Float("view", glm::value_ptr(view));
	bezierShader.setMat4Float("projection", glm::value_ptr(projection));
	faceShader.setFloat("time", time);

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
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	roomModel->DrawTessellated(shader);

	//bezier surface rendering
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, 1.0f, -3.7f));
	surface->Update(time);
	//model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	bezierShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displaceTexture);
	bezierShader.setInt("displaceMap", 0);
	bezierShader.setMat4Float("model", glm::value_ptr(model));
	bezierShader.setMat4Float("normal", glm::value_ptr(glm::inverseTranspose(model)));
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	surface->Draw(bezierShader);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
