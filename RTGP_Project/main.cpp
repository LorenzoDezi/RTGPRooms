#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Skybox.h"
#include "Camera.h"
#include "Model.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"



void framebuffer_size_callback(GLFWwindow *w, int width, int height);
int main();
void generateVAOLamp(unsigned int &VAO_lamp, unsigned int VBO, float  vertices[180]);
void generateVAOCube(unsigned int &VAO_cube, unsigned int VBO, float  vertices[180]);
void mouse_callback(GLFWwindow* w, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *w);


float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = 0.0f;
float lastY = 0.0f;
Camera camera;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(1280, 720, "RTGP_Project", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//Setting mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
	glViewport(0, 0, 1280, 720);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	Shader shader("Shaders/vertex_phong.glsl", "Shaders/fragment_phong.glsl");
	Shader shaderLamp("Shaders/vertex_lamp.glsl", "Shaders/fragment_lamp.glsl");
	Shader skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl");
	glEnable(GL_DEPTH_TEST);
	//For directional light
	glm::vec3 lightDir(0.0f, -0.5f, 0.2f);
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glm::mat4 lightModel(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(0.2f));
	std::string path = "Assets/rooms2.obj";
	Model nanosuitModel(path);
	Model torchModel("Assets/torch2.obj");
	//TODO: Draw instanced
	glm::vec3 lightPositions[]{
		glm::vec3(1.25f, 2.0f, 2.75f),
		glm::vec3(1.25f, 2.0f, 4.90f),
		glm::vec3(1.25f, 2.0f, -4.90f),
		glm::vec3(1.25f, 2.0f, -2.75f),
		glm::vec3(-1.25f, 2.0f, -1.0f),
		glm::vec3(-1.25f, 2.0f, 1.0f)

	};
	//TODO: classe Rotation
	float lightRotations[]{
		0.f,
		0.f,
		0.f,
		0.f,
		135.f,
		135.f
	};
	//Skybox setup
	std::vector<std::string> faces
	{
		"assets/textures/purplenebula_lf.tga",
		"assets/textures/purplenebula_rt.tga",
		"assets/textures/purplenebula_up.tga",
		"assets/textures/purplenebula_dn.tga",
		"assets/textures/purplenebula_ft.tga",
		"assets/textures/purplenebula_bk.tga"
	};
	Skybox skybox(faces);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//frame calc
		float currTime = glfwGetTime();
		deltaTime = currTime - lastFrame;
		lastFrame = currTime;
		processInput(window);
		float time = (float)glfwGetTime();
		glm::mat4 view = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(camera.Zoom, 800.0f / 600.0f, 0.1f, 100.0f);
		shader.use();
		shader.setMat4Float("view", glm::value_ptr(view));
		shader.setMat4Float("projection", glm::value_ptr(projection));
		shaderLamp.use();
		shaderLamp.setMat4Float("view", glm::value_ptr(view));
		shaderLamp.setMat4Float("projection", glm::value_ptr(projection));
		shader.use();
		//for directional light
		shader.setVec3Float("dirLight.direction", lightDir.x, lightDir.y, lightDir.z);
		shader.setVec3Float("dirLight.ambient", 0.15f, 0.15f, 0.15f);
		shader.setVec3Float("dirLight.diffuse", 0.65f, 0.65f, 0.65f);
		shader.setVec3Float("dirLight.specular", 0.65f, 0.65f, 0.65f);

		//Spot light setup
		shader.setVec3Float("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
		shader.setVec3Float("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
		shader.setVec3Float("spotLight.ambient", 0.15f, 0.15f, 0.15f);
		shader.setVec3Float("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shader.setVec3Float("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.f)));

		shader.setVec3Float("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
		shader.setInt("material.diffuse", 0);
		shader.setInt("material.specular", 1);
		shader.setFloat("material.shininess", 0.6f);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setMat4Float("model", glm::value_ptr(model));
		nanosuitModel.Draw(shader);
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPositions[i]);
			model = glm::rotate(model, lightRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));
			shader.setMat4Float("model", glm::value_ptr(model));
			torchModel.Draw(shader);
		}
		skybox.Draw(skyboxShader, view, projection);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void generateVAOLamp(unsigned int &VAO_lamp, unsigned int VBO, float  vertices[180])
{
	glGenVertexArrays(1, &VAO_lamp);
	glBindVertexArray(VAO_lamp);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void generateVAOCube(unsigned int &VAO_cube, unsigned int VBO, float  vertices[180])
{
	glGenVertexArrays(1, &VAO_cube);
	glBindVertexArray(VAO_cube);
	//loading texture
	unsigned int texture, texture1;
	int width, height, nrChannels;
	unsigned char* data;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("assets/container.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture!" << std::endl;
	}
	stbi_image_free(data);
	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("assets/container_specular.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture!" << std::endl;
	}
	stbi_image_free(data);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void framebuffer_size_callback(GLFWwindow *w, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow * w, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *w) {
	if (glfwGetKey(w, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(w, true);
	}
	if (glfwGetKey(w, GLFW_KEY_W)) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(w, GLFW_KEY_S)) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(w, GLFW_KEY_A)) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(w, GLFW_KEY_D)) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}