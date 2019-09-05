#include "Door.h"

Door::Door(sceneType type, glm::vec3 position, Physics &physicsSimulation) : type(type), position(position), model("Assets/door.obj")
{
	btRigidBody* body = physicsSimulation.createRigidBody(BOX, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f), 0.0f, 0.0f, 0.0f);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	body->getBroadphaseProxy()->m_collisionFilterGroup = btBroadphaseProxy::KinematicFilter;
	body->setUserPointer(this);
}

void Door::Draw(Shader shader)
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, position);
	shader.setMat4Float("model", glm::value_ptr(modelMat));
	model.Draw(shader);
}

sceneType Door::getSceneType()
{
	return type;
}

