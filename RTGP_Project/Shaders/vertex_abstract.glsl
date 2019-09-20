#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;

out vec3 Pos_CS;
out vec2 TexCoords_CS;
out vec3 Normal_CS;

void main() {
	//There is no need to transform in clip space, 
	//a lot of vertices will be generated (and they will need that transformation)
	Pos_CS = vec3(model * vec4(aPos, 1.0));
	TexCoords_CS = texCoords;
	Normal_CS = mat3(inverse(transpose(model))) * aNormal;
}