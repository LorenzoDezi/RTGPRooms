#include "Camera.h"

// Constructor with vectors
Camera::Camera(Physics physicsSimulation, glm::vec3 position, glm::vec3 size, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	//TODO Create cylinder shape rigid body
	rigidbody = physicsSimulation.createRigidBody(CAPSULE, position, size, 
		glm::vec3(0.1f, 0.0f, 0.1f), 50.0f, 0.2f, 0.0f);
	rigidbody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rigidbody->setActivationState(DISABLE_DEACTIVATION);
	updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
	updateCameraVectors();
	return glm::lookAt(Position, Position + Front, Up);
}


void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float forceIntensity = 15.0f;
	//TODO: Set maximum velocity
	glm::vec3 forceVector;
	if (direction == FORWARD)
		forceVector = WorldFront * forceIntensity;
	if (direction == BACKWARD)
		forceVector = -WorldFront * forceIntensity;
	if (direction == LEFT)
		forceVector = -Right * forceIntensity;
	if (direction == RIGHT)
		forceVector = Right * forceIntensity;
	btVector3 btForceVector(forceVector.x, forceVector.y, forceVector.z);
	rigidbody->applyCentralForce(btForceVector);
	updateCameraVectors();
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Calculates the front vector from the Camera's (updated) Euler Angles

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	btVector3 translation = rigidbody->getWorldTransform().getOrigin();
	Position = glm::vec3(translation.x(), translation.y(), translation.z());
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	WorldFront = glm::vec3(Front.x, 0.0f, Front.z);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}

glm::mat4 Camera::GetModelMatrix()
{
	// we upcast it in order to use the methods of the main class RigidBody
	btRigidBody* body = btRigidBody::upcast(rigidbody);
	btTransform transform;
	// we take the transformation matrix of the rigid boby, as calculated by the physics engine
	GLfloat matrix[16];
	body->getMotionState()->getWorldTransform(transform);
	transform.getOpenGLMatrix(matrix);
	return glm::make_mat4(matrix);
}
