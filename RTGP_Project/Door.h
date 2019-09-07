#pragma once

#include "Model.h"
#include "Physics.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum sceneType { CORRIDOR, NATURAL, TOON, ABSTRACT };

class Door
{
public:
	Door(sceneType type, glm::vec3 position, glm::vec3 collisionPos, Physics &physicsSimulation);
	void Draw(Shader &shader);
	sceneType getSceneType();

private:
	sceneType type;
	Model model;
	glm::vec3 position;
};

