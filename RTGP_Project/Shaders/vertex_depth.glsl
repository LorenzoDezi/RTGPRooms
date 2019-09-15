#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 texCoords;

out vec2 TexCoords;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
	TexCoords = texCoords;
}