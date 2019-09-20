#version 430 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec2 TexCoords;
in float truchetOffset;
in vec3 Normal;

uniform float time;

void main() {
	vec3 color = vec3(0.0);
	color += vec3(0.70, 0.11, 0.32) * truchetOffset * 2.0;
	FragColor = vec4(color, 1.0);

	if (FragColor.r >= 0.1)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}