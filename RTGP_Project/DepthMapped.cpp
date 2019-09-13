#include "DepthMapped.h"



void DepthMapped::setDepthMap(GLuint depthMap)
{
	this->depthMap = depthMap;
}

void DepthMapped::bindDepthMap(Shader &shader)
{
	//In every shader, the first texture bound is reserved to the depth map
	shader.setInt("depthMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

DepthMapped::DepthMapped()
{
}

