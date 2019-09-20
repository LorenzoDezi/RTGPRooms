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

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 albedo);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float ShadowCalculation();
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main()
{
	vec3 V = normalize(viewPos - FragPos);
	vec3 N = vec3(0.0, 0.0, -1.0);
	vec3 Lo = vec3(0.0, 0.0, 0.0);
	vec4 TexColor = texture2D(materials[TexIndex].tex, vec2(TexCoord.x, -TexCoord.y));
	if (TexColor.a < 0.2)
		discard;
	//TODO find a way if there is time, stop bitching
	/*for (int i = 0; i < NR_DIR_LIGHTS; i++) {
		Lo += CalcDirLight(dirLight[i], N, V, TexColor.rgb);
	}
	if (newAlpha > 1.0f)
		newAlpha = 1.0f;
	FragColor = vec4(Lo.xyz, newAlpha);*/
	float shadowWeight = ShadowCalculation();
	FragColor = vec4(TexColor.xyz * ColorWeight * (1 - shadowWeight), 1.0);
	//DEBUG
	//FragColor = vec4(shadowWeight, 0, 0, 1.0);
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
			shadow += currentDepth - bias > pcfDepth ? 0.6 : 0.0;
		}
	}
	shadow /= 9.0;
	return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 albedo) {
	vec3 L = normalize(light.direction);
	float NdotL = max(dot(norm, L), 0.0);
	vec3 Lo = vec3(0.0);
	vec3 H = normalize(viewDir + L);
	vec3 radiance = light.color;
	//For non metallic surface, F0 for fresnel is always 0.04.
	vec3 F0 = vec3(0.04, 0.04, 0.04);
	//Calculating the various components of the rendering equation
	vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);
	float NDF = DistributionGGX(norm, H, roughness);
	float G = GeometrySmith(norm, viewDir, L, roughness);
	//Actually using those components
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(norm, viewDir), 0.0) * max(dot(norm, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.001);
	//F directly corresponds to Ks and for the law of energy conservation, we
	//can easily find Kd.
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	//Summing the contribution of the light
	Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	return Lo;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}