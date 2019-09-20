#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in mat4 instanceMatrix;
layout(location = 5) in int textureIndex;

out mat4 modelMatrix;
flat out int texIndex;


void main()
{
	gl_Position = vec4(aPos, 1.0);
	modelMatrix = instanceMatrix;
	texIndex = textureIndex;
}