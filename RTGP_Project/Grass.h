#pragma once
#include "Shader.h"
#include "stb_image/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

class Grass
{
public:
	Grass();
	void Draw(glm::vec3 position, glm::mat4 view, glm::mat4 projection, float time);
	void DrawInstanced(glm::mat4 view, glm::mat4 projection, float time);
	GLuint TextureFromFile(const char * path, const std::string & directory, bool gamma);
	~Grass();

private:
	Shader grassShader;
	GLuint VBO, VAO;
	GLuint grassTexture;

};

