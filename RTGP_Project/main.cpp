#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "CorridorScene.h"
#include "NaturalScene.h"
#include "AbstractScene.h"
#include "ToonScene.h"
#include "BlinnPhongModel.h"
#include "Physics.h"
#include "RenderQuad.h"
#include "Skybox.h"
#include "Camera.h"
#include "Door.h"
#include "Model.h"
#include <iostream>
#include <unordered_map>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#define NR_POINT_LIGHTS 6
#define SCR_WIDTH 1920.0f
#define SCR_HEIGHT 1080.0f

void framebuffer_size_callback(GLFWwindow *w, int width, int height);
int main();
void generateColorBuffers(GLuint &hdrFBO, GLuint colorBuffer[]);
void generateBlurPingPongBuffers(GLuint pingpongFBO[], GLuint pingpongBuffer[]);
GLuint getBlurredBuffer(Shader &shaderBlur, GLuint &colorBuffer, GLuint pingpongFBO[], 
	GLuint pingpongBuffer[], RenderQuad& quad);
void mouse_callback(GLFWwindow* w, double xpos, double ypos);
void processInput(GLFWwindow *w);

void showFrameRate();

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//DEBUG
GLuint framesPerSecond = 0;
double lastTime = 0.0f;
GLfloat exposure = 1.0f;

GLboolean firstMouse = true;
GLfloat lastX = 0.0f;
GLfloat lastY = 0.0f;
Physics physicsSimulation;
GLfloat maxSecPerFrame = 1.0f / 60.0f;
Camera camera(physicsSimulation, glm::vec3(0.0f, 15.0f, 0.0f));
std::unordered_map<sceneType, std::unique_ptr<Scene>> sceneMap;


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RTGP_Project", NULL, NULL);
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
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//TODO Calculate here normal matrix, on the CPU, because it costs on the vertex shader
	//TODO Uniform buffer objects for lights
	glEnable(GL_DEPTH_TEST);

	//hdr and bloom shaders setup
	Shader hdrShader("Shaders/vertex_hdr.glsl", "Shaders/fragment_hdr.glsl");
	hdrShader.use();
	hdrShader.setInt("hdrBuffer", 0);
	hdrShader.setInt("bloomBlur", 1);
	Shader shaderBlur("Shaders/vertex_hdr.glsl", "Shaders/fragment_blur.glsl");
	shaderBlur.use();
	shaderBlur.setInt("image", 0);
	// generating floating point framebuffers to store hdr values and brights colors (Bloom)
	GLuint hdrFBO;
	GLuint colorBuffers[2];
	generateColorBuffers(hdrFBO, colorBuffers);
	GLuint pingpongFBO[2], pingpongBuffer[2];
	generateBlurPingPongBuffers(pingpongFBO, pingpongBuffer);
	RenderQuad hdrQuad;

	//Scene setup
	//Rooms
	Model roomModel("Assets/rooms2.obj");
	//Physics setup
	physicsSimulation.createStaticRigidBodyWithTriangleMesh(roomModel, glm::vec3(), glm::vec3(),
		glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);

	//Doors
	std::vector<std::shared_ptr<Door>> doors {
		std::make_shared<Door>(CORRIDOR, glm::vec3(0.01f, 0.0f, 0.0f), glm::vec3(-1.38f, 1.5f, -0.09f), physicsSimulation),
		std::make_shared<Door>(CORRIDOR, glm::vec3(0.01f, 0.0f, 0.0f), glm::vec3(-1.38f, 1.5f, -0.09f), physicsSimulation),
		std::make_shared<Door>(CORRIDOR, glm::vec3(2.94f, 0.0, 3.83f), glm::vec3(1.32f, 1.5f, 3.96f), physicsSimulation),
		std::make_shared<Door>(TOON, glm::vec3(3.08f, 0.0f, 3.83f), glm::vec3(1.52f, 1.5f, 3.96f), physicsSimulation),
		std::make_shared<Door>(CORRIDOR, glm::vec3(2.94f, 0.0, -3.83f), glm::vec3(1.32f, 1.5f, -3.72f), physicsSimulation),
		std::make_shared<Door>(ABSTRACT, glm::vec3(3.08f, 0.0f, -3.83f), glm::vec3(1.52f, 1.5f, -3.72f), physicsSimulation),
		std::make_shared<Door>(NATURAL, glm::vec3(-0.15f, 0.0f, 0.0f), glm::vec3(-1.58f, 1.5f, -0.09f), physicsSimulation)
	};
	//Map scenetype -> room
	sceneType currentSceneType = CORRIDOR;
	sceneMap[CORRIDOR] = std::unique_ptr<Scene>(new CorridorScene(physicsSimulation, roomModel, doors, SCR_WIDTH, SCR_HEIGHT));
	sceneMap[NATURAL] = std::unique_ptr<Scene>(new NaturalScene(physicsSimulation, roomModel, doors, SCR_WIDTH, SCR_HEIGHT));
	//DEBUG - change later
	//sceneMap[NATURAL] = std::unique_ptr<Scene>(new CorridorScene(physicsSimulation, roomModel, doors));
	//TODO: Implement toon and abstract classes
	sceneMap[TOON] = std::unique_ptr<Scene>(new ToonScene(physicsSimulation, roomModel, doors, SCR_WIDTH, SCR_HEIGHT));
	sceneMap[ABSTRACT] = std::unique_ptr<Scene>(new AbstractScene(physicsSimulation, roomModel, doors, SCR_WIDTH, SCR_HEIGHT));
	
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//frame calc
		float currTime = glfwGetTime();
		deltaTime = currTime - lastFrame;
		lastFrame = currTime;

		//Updating physics simulation
		physicsSimulation.dynamicsWorld->stepSimulation((deltaTime < maxSecPerFrame ? deltaTime : maxSecPerFrame), 10);
		
		btCollisionWorld::ClosestRayResultCallback rayCallback(
			btVector3(camera.Position.x, camera.Position.y, camera.Position.z),
			btVector3(camera.WorldFront.x, camera.WorldFront.y, camera.WorldFront.z) * 0.01f
		);
		rayCallback.m_collisionFilterMask = btBroadphaseProxy::KinematicFilter;
		physicsSimulation.dynamicsWorld->rayTest(
			btVector3(camera.Position.x, camera.Position.y, camera.Position.z),
			btVector3(camera.WorldFront.x, camera.WorldFront.y, camera.WorldFront.z) * 0.01f,
			rayCallback);
		if (rayCallback.hasHit()) {
			sceneType *val = static_cast<sceneType *>(rayCallback.m_collisionObject->getUserPointer());
			currentSceneType = *val;
			//DEBUG
			std::cout << "Crossed -> " << *val << std::endl;
		}

		processInput(window);
		//Rendering into the floating point framebuffer for hdr
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sceneMap[currentSceneType]->Draw(camera, currTime);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//Rendering floating point color buffer to 2D quad
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (sceneMap[currentSceneType]->hasBloom()) {
			GLuint blurredBuffer = getBlurredBuffer(shaderBlur, colorBuffers[1],
				pingpongFBO, pingpongBuffer, hdrQuad);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, blurredBuffer);
		}
		else {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		}
		hdrShader.use();
		hdrShader.setBool("hasBloom", sceneMap[currentSceneType]->hasBloom());
		hdrShader.setFloat("exposure", exposure);
		hdrQuad.Draw();
		//Swap double buffer and poll events
		glfwSwapBuffers(window);
		//DEBUG
		//showFrameRate();
		glfwPollEvents();
	}
	physicsSimulation.Clear();
	glfwTerminate();
	return 0;
}

void generateColorBuffers(GLuint &hdrFBO, GLuint colorBuffers[])
{
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	//Now the frame buffer will render to those two attachments
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void generateBlurPingPongBuffers(GLuint pingpongFBO[], GLuint pingpongBuffer[])
{
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
		);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}
}

GLuint getBlurredBuffer(Shader& shaderBlur, GLuint& colorBuffer, GLuint pingpongFBO[], GLuint pingpongBuffer[], 
	RenderQuad& quad)
{
	bool horizontal = true, first_iteration = true;
	int amount = 10;
	shaderBlur.use();
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		shaderBlur.setInt("horizontal", horizontal);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? colorBuffer : pingpongBuffer[!horizontal]
		);
		quad.Draw();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return pingpongBuffer[!horizontal];
}

void framebuffer_size_callback(GLFWwindow *w, int width, int height) {
	glViewport(0, 0, width, height);
	//TODO: windows resize, change render quad and blur framebuffers size
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
	//DEBUG
	if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS) {
		std::cout << camera.Position.x << "-" << camera.Position.y << "-" << camera.Position.z << std::endl;
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

