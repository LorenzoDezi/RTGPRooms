﻿#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

struct DirLight {
	vec3 direction;
	vec3 color;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
#define NR_DIR_LIGHTS 1
uniform DirLight dirLight[NR_DIR_LIGHTS];

out vec3 Normal;
//All the calculation of the lightning is done in world space,
//so we need the world space of the light
out vec3 FragPos;
out vec4 LightSpaceFragPos;
out mat3 TBN;
out vec2 TexCoords;
out DirLight FragDirLight[NR_DIR_LIGHTS];

void main() {
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	LightSpaceFragPos = lightSpaceMatrix * vec4(FragPos, 1.0);
	TexCoords = texCoords;
	Normal = mat3(inverse(transpose(model))) * aNormal;
	//Calculating TBN matrix for tangent space
	vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	TBN = mat3(T, B, N);
	for (int i = 0; i < NR_DIR_LIGHTS; i++) {
		FragDirLight[i].direction = TBN * dirLight[i].direction;
		FragDirLight[i].color = dirLight[i].color;
	}
}