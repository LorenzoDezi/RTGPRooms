#pragma once
#include "glad/glad.h"
#include "Shader.h"
class DepthMapped
{
public:
	void setDepthMap(GLuint depthMap);
	void bindDepthMap(Shader &shader);
protected:
	DepthMapped();
private:
	GLuint depthMap;
};

