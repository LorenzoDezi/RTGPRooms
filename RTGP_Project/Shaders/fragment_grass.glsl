#version 330 core

in vec2 TexCoord;

//TODO: Vedere se servono effettivamente poi per l'illuminazione
//ma dovrebbero beh si
in vec3 WorldPos;
in vec4 EyeSpacePos;
uniform vec3 EyePosition;

out vec4 FragColor;
//TODO: Try to implement random texture selection (between your grass leaves)
uniform sampler2D texture;

uniform float alphaTest;
uniform float alphaMultiplier;

void main()
{
	vec4 TexColor = texture2D(texture, vec2(TexCoord.x, -TexCoord.y));
	float newAlpha = TexColor.a*alphaMultiplier;
	if (newAlpha < alphaTest)
		discard;

	if (newAlpha > 1.0f)
		newAlpha = 1.0f;
	//DEBUG
	FragColor = vec4(TexColor.xyz, 1.0);
}