#pragma once
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "Shader.h"
#include "Camera.h"
#include <vector>

class BezierSurface
{
public:
	BezierSurface(std::vector<glm::vec3> &controlPoints);
	void Update(float time);
	void Draw(Shader &shader);
	~BezierSurface();

private:
	GLuint VAO, VBO;
	std::vector<glm::vec3> controlPoints;
};

