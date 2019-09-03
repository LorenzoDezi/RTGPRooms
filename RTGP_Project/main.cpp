#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "BlinnPhongModel.h"
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
void processInput(GLFWwindow *w);
//DEBUG
void showFrameRate();

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//DEBUG
GLuint framesPerSecond = 0;
double lastTime = 0.0f;

GLboolean firstMouse = true;
GLfloat lastX = 0.0f;
GLfloat lastY = 0.0f;
Physics physicsSimulation;
GLfloat maxSecPerFrame = 1.0f / 60.0f;
Camera camera(physicsSimulation, glm::vec3(0.0f, 15.0f, 0.0f));

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
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
	glViewport(0, 0, 1280, 720);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Shaders setup
	Shader shader("Shaders/vertex_phong.glsl", "Shaders/fragment_phong.glsl");
	Shader shaderLight("Shaders/vertex_lamp.glsl", "Shaders/fragment_lamp.glsl");
	Shader skyboxShader("Shaders/vertex_skybox.glsl", "Shaders/fragment_skybox.glsl");

	//Models setup
	Model roomModel("Assets/rooms2.obj");
	Model torchModel("Assets/torch2.obj");
	Model sphereModel("Assets/sphere.obj");

	//Physics setup
	physicsSimulation.createStaticRigidBodyWithTriangleMesh(roomModel, glm::vec3(), glm::vec3(), 
		glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);

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
		float time = (float)glfwGetTime();

		// we update the physics simulation. We must pass the deltatime to be used for the update of the physical state of the scene. 
		//The default value for Bullet is 60 Hz, for lesser deltatime the library interpolates and does not calculate the simulation. 
		//In this example, we use deltatime from the last rendering: if it is < 1\60 sec, than we use it, otherwise we use the deltatime 
		//we have set above. we also set the max number of substeps to consider for the simulation (=10)
		physicsSimulation.dynamicsWorld->stepSimulation((deltaTime < maxSecPerFrame ? deltaTime : maxSecPerFrame), 10);
		
		processInput(window);
		//Shader common setup
		BlinnPhongModel blinnPhongModel(shader);
		glm::mat4 view = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(camera.Zoom, 800.0f / 600.0f, 0.1f, 100.0f);
		shader.use();
		shader.setMat4Float("view", glm::value_ptr(view));
		shader.setMat4Float("projection", glm::value_ptr(projection));
		shader.setVec3Float("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
		blinnPhongModel.setLightParameters(
			glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.02f, 0.02f, 0.02f));
		blinnPhongModel.setDirLight(lightDir);

		shaderLight.use();
		shaderLight.setMat4Float("view", glm::value_ptr(view));
		shaderLight.setMat4Float("projection", glm::value_ptr(projection));

		//lights rendering
		blinnPhongModel.setLightParameters(glm::vec3(0.15f, 0.15f, 0.15f),
			glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.5f, 0.5f, 0.5f));
		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			blinnPhongModel.setPointLight(pointLightPositions[i], i);
			shaderLight.use();
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, lightSupportPositions[i] + glm::vec3(0.0f, 0.75f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2));
			shaderLight.setMat4Float("model", glm::value_ptr(model));
			sphereModel.Draw(shaderLight);
		}
		//light supports rendering
		blinnPhongModel.setMaterial(0.6f, 0.6f, 0.0f, 0.2f);
		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, lightSupportPositions[i]);
			model = glm::rotate(model, lightSupportRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));
			shader.use(); 
			shader.setMat4Float("model", glm::value_ptr(model));
			torchModel.Draw(shader);
		}

		//room rendering
		blinnPhongModel.setMaterial(0.6f, 0.6f, 0.1f, 0.2f);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		shader.use();
		shader.setMat4Float("model", glm::value_ptr(model));
		roomModel.Draw(shader);
		
		//skybox rendering
		skybox.Draw(skyboxShader, view, projection);

		//Swap double buffer and poll events
		glfwSwapBuffers(window);
		//DEBUG
		showFrameRate();
		glfwPollEvents();
	}
	physicsSimulation.Clear();
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

void processInput(GLFWwindow *w) {
	if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(w, true);
	}
	if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD);
	}
	if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD);
	}
	if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT);
	}
	if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT);
	}
}

void showFrameRate()
{
	double currentTime = glfwGetTime();
	++framesPerSecond;
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		std::cout << "\nCurrent Frames Per Second: " << framesPerSecond;
		framesPerSecond = 0;
	}
}

