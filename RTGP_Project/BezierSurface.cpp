#include "BezierSurface.h"


BezierSurface::BezierSurface(std::vector<glm::vec3> &controlPoints) : 
	controlPoints(controlPoints)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec3), &controlPoints[0], GL_DYNAMIC_DRAW);
	//Control points positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindVertexArray(0);
}

void BezierSurface::Update(float time)
{
	srand(NULL);
	for (int i = 0; i < 4; i++) {
		int sign = i % 2 == 0 ? 1 : -1;
		for (int j = 0; j < 4; j++) {
			int j_sign = j % 2 == 0 ? 1 : -1;
			controlPoints[i * 4 + j].z += cos(time * 3.5) * 0.006f * j_sign;
			controlPoints[i * 4 + j].y += cos(time * 3) * 0.02f * -sign;
		}
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec3), &controlPoints[0], GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
}

void BezierSurface::Draw(Shader &shader)
{
	//TODO: see if shader parameter is needed
	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glBindVertexArray(VAO);
	//DEBUG
	glDrawArrays(GL_PATCHES, 0, controlPoints.size());
	glBindVertexArray(0);
}

BezierSurface::~BezierSurface()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
