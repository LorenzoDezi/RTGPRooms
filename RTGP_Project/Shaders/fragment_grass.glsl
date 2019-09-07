#version 330 core

in vec2 TexCoord;

//TODO: Vedere se servono effettivamente poi per l'illuminazione
//ma dovrebbero beh si
in vec3 WorldPos;
flat in int TexIndex;
in vec4 EyeSpacePos;
uniform vec3 EyePosition;

out vec4 FragColor;
//TODO: Try to implement random texture selection (between your grass leaves)
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;
uniform sampler2D texture8;

uniform float alphaTest;
uniform float alphaMultiplier;

void main()
{
	vec4 TexColor;
	switch (TexIndex) {
	case 0:
		TexColor = texture2D(texture0, vec2(TexCoord.x, -TexCoord.y));
		break;
	case 1:
		TexColor = texture2D(texture1, vec2(TexCoord.x, -TexCoord.y));
		break;
	case 2:
		TexColor = texture2D(texture2, vec2(TexCoord.x, -TexCoord.y));
		break;
	case 3:
		TexColor = texture2D(texture3, vec2(TexCoord.x, -TexCoord.y));
		break;
	case 4:
		TexColor = texture2D(texture4, vec2(TexCoord.x, -TexCoord.y));
		break;
	case 5:
		TexColor = texture2D(texture5, vec2(TexCoord.x, -TexCoord.y));
		break;
	case 6:
		TexColor = texture2D(texture6, vec2(TexCoord.x, -TexCoord.y));
		break;
	case 7:
		TexColor = texture2D(texture7, vec2(TexCoord.x, -TexCoord.y));
		break;
	case 8:
		TexColor = texture2D(texture8, vec2(TexCoord.x, -TexCoord.y));
		break;
	}
	float newAlpha = TexColor.a*alphaMultiplier;
	if (newAlpha < alphaTest)
		discard;

	if (newAlpha > 1.0f)
		newAlpha = 1.0f;
	//DEBUG
	FragColor = vec4(TexColor.xyz, 1.0);
}