#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Physics.h"
#include <iostream>
#include <vector>
#include <unordered_map>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float MAX_VELOCITY = 5.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Thrust;
	glm::vec3 WorldFront;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	std::unordered_map<Camera_Movement, bool> inputMap;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	//Camera's rigidbody
	btRigidBody* rigidbody;

	// Constructor with vectors
	Camera(Physics physicsSimulation, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3 size = glm::vec3(0.4f, 0.5f, 0.4f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW, float pitch = PITCH);
	
	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();

	// Returns the model matrix calculated from the physics engine
	glm::mat4 GetModelMatrix();

	void ProcessKeyboard(Camera_Movement direction);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles and the position from the physics engine
	void updateCameraVectors();
};
