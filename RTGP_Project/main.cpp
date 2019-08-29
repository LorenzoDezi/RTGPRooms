#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Physics.h"
#include "Skybox.h"
#include "Camera.h"
#include "Model.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#define NR_POINT_LIGHTS 6



void framebuffer_size_callback(GLFWwindow *w, int width, int height);
int main();
void mouse_callback(GLFWwindow* w, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *w);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = 0.0f;
float lastY = 0.0f;
Camera camera(glm::vec3(0.0f, 1.5f, 0.0f));
Physics physicsSimulation;

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

	//Shaders setup
	Shader shader("Shaders/vertex_phong.glsl", "Shaders/fragment_phong.glsl");
	Shader shaderLamp("Shaders/vertex_lamp.glsl", "Shaders/fragment_lamp.glsl");
	Shader skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl");
	//Models setup
	Model roomModel("Assets/rooms2.obj");
	Model torchModel("Assets/torch2.obj");
	Model sphereModel("Assets/sphere.obj");
	//Light positions setup
	glm::vec3 lightSupportPositions[]{
		glm::vec3(1.25f, 2.0f, 2.75f),
		glm::vec3(1.25f, 2.0f, 4.90f),
		glm::vec3(1.25f, 2.0f, -4.90f),
		glm::vec3(1.25f, 2.0f, -2.75f),
		glm::vec3(-1.25f, 2.0f, -1.0f),
		glm::vec3(-1.25f, 2.0f, 1.0f)
	};
	glm::vec3 pointLightPositions[] {
		glm::vec3(1.95f, 2.75f, 2.75f),
		glm::vec3(1.95f, 2.75f, 4.90f),
		glm::vec3(1.95f, 2.75f, -4.90f),
		glm::vec3(1.95f, 2.75f, -2.75f),
		glm::vec3(-1.95f, 2.75f, -1.0f),
		glm::vec3(-1.95f, 2.75f, 1.0f)
	};
	float lightSupportRotations[] {
		0.f, 0.f, 0.f, 0.f, 135.f, 135.f
	};
	glm::vec3 lightDir(0.0f, -1.0f, 0.0f);
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
	//TODO Calculate here normal matrix, on the CPU, because it costs on the vertex shader
	//TODO Uniform buffer objects for lights
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//frame calc
		float currTime = glfwGetTime();
		deltaTime = currTime - lastFrame;
		lastFrame = currTime;
		processInput(window);
		float time = (float)glfwGetTime();

		//Shader common setup
		glm::mat4 view = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(camera.Zoom, 800.0f / 600.0f, 0.1f, 100.0f);
		shader.use();
		shader.setMat4Float("view", glm::value_ptr(view));
		shader.setMat4Float("projection", glm::value_ptr(projection));
		shader.setVec3Float("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
		shader.setVec3Float("dirLight.direction", lightDir.x, lightDir.y, lightDir.z);
		shader.setVec3Float("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3Float("dirLight.diffuse", 0.1f, 0.1f, 0.1f);
		shader.setVec3Float("dirLight.specular", 0.02f, 0.02f, 0.02f);

		shaderLamp.use();
		shaderLamp.setMat4Float("view", glm::value_ptr(view));
		shaderLamp.setMat4Float("projection", glm::value_ptr(projection));

		//lights rendering
		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			shader.use();
			std::string refLight = "pointLights[" + std::to_string(i) + "]";
			//TODO add 0.65 in relative positions
			shader.setVec3Float(refLight + ".position", pointLightPositions[i].x, 
				pointLightPositions[i].y, pointLightPositions[i].z);
			shader.setFloat(refLight + ".constant", 1.0f);
			shader.setFloat(refLight + ".linear", 0.22f);
			shader.setFloat(refLight + ".quadratic", 0.20f);
			shader.setVec3Float(refLight + ".ambient", 0.15f, 0.15f, 0.15f);
			shader.setVec3Float(refLight + ".diffuse", 0.8f, 0.8f, 0.8f);
			shader.setVec3Float(refLight + ".specular", 0.1f, 0.1f, 0.1f);
			shaderLamp.use();
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, lightSupportPositions[i] + glm::vec3(0.0f, 0.75f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2));
			shaderLamp.setMat4Float("model", glm::value_ptr(model));
			sphereModel.Draw(shaderLamp);
		}
		//light supports rendering
		shader.use();
		shader.setInt("material.diffuse", 0);
		shader.setInt("material.specular", 1);
		shader.setFloat("material.shininess", 0.5f);
		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, lightSupportPositions[i]);
			model = glm::rotate(model, lightSupportRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));
			shader.setMat4Float("model", glm::value_ptr(model));
			torchModel.Draw(shader);
		}

		//room rendering
		shader.use();
		shader.setInt("material.diffuse", 0);
		shader.setInt("material.specular", 1);
		shader.setFloat("material.shininess", 0.5f);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setMat4Float("model", glm::value_ptr(model));
		roomModel.Draw(shader);
		
		//skybox rendering
		skybox.Draw(skyboxShader, view, projection);

		//Swap double buffer and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
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