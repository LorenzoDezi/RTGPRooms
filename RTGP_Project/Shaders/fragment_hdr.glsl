#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float exposure;
uniform sampler2D hdrBuffer;

void main()
{
	const float gamma = 1.0;
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	// reinhard tone mapping
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	// gamma correction 
	result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(result, 1.0);
}