#pragma once
#include "stb_image/stb_image.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include <vector>
#include <iostream>

class Skybox
{
public:
	Skybox(std::string faces[]);
	void Draw(Shader &shader, glm::mat4 view, glm::mat4 projection);
	GLuint getTexture();
	virtual ~Skybox();
private:
	GLuint skyVAO, skyVBO, textureID;
};

