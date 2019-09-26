#version 430 core

in vec2 TexCoord;
in float ColorWeight;
in vec3 FragPos;
in vec3 Normal;
in vec4 LightSpaceFragPos;
flat in int TexIndex;

struct Material {
	sampler2D tex;
};

struct DirLight {
	vec3 direction;
	vec3 color;
};

out vec4 FragColor;
#define NR_MATERIALS 9
uniform Material materials[NR_MATERIALS];
#define NR_DIR_LIGHTS 2
uniform DirLight dirLight[NR_DIR_LIGHTS];
uniform vec3 viewPos;
uniform float roughness;
uniform sampler2D depthMap;

const float PI = 3.14159265359;

float ShadowCalculation();

void main()
{
	vec3 V = normalize(viewPos - FragPos);
	vec3 N = vec3(0.0, 0.0, -1.0);
	vec3 Lo = vec3(0.0, 0.0, 0.0);
	vec4 TexColor = texture2D(materials[TexIndex].tex, vec2(TexCoord.x, -TexCoord.y));
	if (TexColor.a < 0.2)
		discard;
	float shadowWeight = ShadowCalculation();
	FragColor = vec4(TexColor.xyz * ColorWeight * (1 - shadowWeight), 1.0);
}

float ShadowCalculation() {
	// perform perspective divide
	vec3 projCoords = LightSpaceFragPos.xyz / LightSpaceFragPos.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(depthMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float bias = 0.0001;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 0.7 : 0.0;
		}
	}
	shadow /= 9.0;
	return shadow;
}
