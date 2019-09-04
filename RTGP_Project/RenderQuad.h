#pragma once
#include <glad/glad.h>

class RenderQuad
{
public:
	RenderQuad();
	void Draw();
	~RenderQuad();
private:
	GLuint quadVAO;
	GLuint quadVBO;
};

