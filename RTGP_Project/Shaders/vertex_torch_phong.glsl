#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
//All the calculation of the lightning is done in world space,
//so we need the world space of the light
out vec3 FragPos;
out vec2 TexCoords;

void main() {
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoords = texCoords;
	//For simplicity we calculate the normal matrix here but it is better to
	//calculate in the CPU and then pass it via uniform, because it's costly
	//for each vertex
	Normal = mat3(inverse(transpose(model))) * aNormal;
}