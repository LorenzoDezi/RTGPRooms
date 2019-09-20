#version 430 core
layout(location = 0) in vec3 aPosition;

out vec4 Pos_CS;

void main() {
	Pos_CS = vec4(aPosition, 1.0);
}