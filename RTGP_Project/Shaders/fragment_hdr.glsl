#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;
uniform bool hasBloom;
uniform float exposure;

void main()
{
	const float gamma = 1.0;
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	if (hasBloom) {
		vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
		hdrColor += bloomColor; // additive blending
	}
	// tone mapping
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	// gamma correction 
	result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(result, 1.0);
}