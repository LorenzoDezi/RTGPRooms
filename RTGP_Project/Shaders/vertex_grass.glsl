#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in mat4 instanceMatrix;

out mat4 modelMatrix;


void main()
{
	gl_Position = vec4(aPos, 1.0);
	modelMatrix = instanceMatrix;
}