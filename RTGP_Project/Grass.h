#pragma once
#include "Shader.h"
#include "ImageUtility.h"
#include "stb_image/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "time.h"
#include <vector>

#define NR_GRASS_TEXTURES 9

typedef struct {
	glm::mat4 modelMatrix;
	GLint textureIndex;
} GrassInput;

class Grass
{
public:
	Grass();
	void setPositions(const std::vector<glm::vec3> &positions);
	void Draw(const glm::mat4 &view, const glm::mat4 &projection, float time);
	~Grass();

private:
	void initTextures();

private:
	Shader grassShader;
	GLuint VBO, VAO, instancingVBO;
	GLuint grassTextures[NR_GRASS_TEXTURES];
	GLuint amount;

};
