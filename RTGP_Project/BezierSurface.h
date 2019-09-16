#pragma once
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "Shader.h"
#include "Camera.h"
#include <vector>

class BezierSurface
{
public:
	BezierSurface(std::vector<glm::vec3> &controlPoints, std::vector<GLuint> &controlPointIndices);
	void Update(float time);
	void Draw(Shader &shader);
	~BezierSurface();

private:
	GLuint VAO, VBO, EBO;
	std::vector<glm::vec3> controlPoints;
	std::vector<GLuint> controlPointIndices;
};

