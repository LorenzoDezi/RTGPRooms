#version 420 core

in vec2 TexCoords;

struct Material {
	sampler2D texture_diffuse;
};

uniform Material material;

void main()
{
	//Check for transparency
	float alpha = texture(material.texture_diffuse, TexCoords).a;
	if (alpha < 0.6)
		discard;
	//Already done by default
	gl_FragDepth = gl_FragCoord.z;
}