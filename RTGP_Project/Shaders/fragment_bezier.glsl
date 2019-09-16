#version 420 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 norm;
in float DisplaceFactor;

void main() {
	//TODO
	/*if (DisplaceFactor <= 0.0)
		discard;*/
	vec3 color = vec3(0.86, 0.24, 0.0);
	color += vec3(0.4, 0.4, 0.4) * DisplaceFactor;
	
	FragColor = vec4(color, 1.0);

	if (FragColor.r >= 2.5)
		BrightColor = vec4(color, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}