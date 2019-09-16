#include "BezierSurface.h"


BezierSurface::BezierSurface(std::vector<glm::vec3> &controlPoints, std::vector<GLuint> &controlPointIndices) : 
	controlPoints(controlPoints), controlPointIndices(controlPointIndices)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec3), &controlPoints[0], GL_DYNAMIC_DRAW);
	//DEBUG
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, controlPointIndices.size() * sizeof(GLuint), &controlPointIndices[0], GL_DYNAMIC_DRAW);

	//Control points positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindVertexArray(0);
}

void BezierSurface::Update(float time)
{
	srand(NULL);
	for (int i = 0; i < controlPoints.size(); i++) {
		if (i % 2 != 0) {
			controlPoints[i].x += cos(time) * 0.0007f;
		}
		else {
			controlPoints[i].y += sin(time)* 0.0007f;
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
	glDrawArrays(GL_PATCHES, 0, controlPoints.size());
	//glDrawElements(GL_PATCHES, controlPointIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

BezierSurface::~BezierSurface()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
