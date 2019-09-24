#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vec3 translatedPos = aPos + 0.005 * normalize(aNormal);
	gl_Position = projection * view * model * vec4(translatedPos.x, translatedPos.y, translatedPos.z, 1.0);
}