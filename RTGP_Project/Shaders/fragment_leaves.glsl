﻿#version 430 core

struct Material {
	sampler2D texture_diffuse;
	sampler2D texture_specular;
	sampler2D texture_normals;
	float metallic;
	sampler2D texture_roughness;
	sampler2D texture_ao;
};

struct DirLight {
	vec3 direction;
	vec3 color;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
#define NR_DIR_LIGHTS 2
uniform DirLight dirLight[NR_DIR_LIGHTS];

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 albedo);

void main() {
	float alpha = texture(material.texture_diffuse, TexCoords).a;
	if (alpha < 0.6)
		discard;
	vec3 albedo = vec3(texture(material.texture_diffuse, TexCoords));
	vec3 color = albedo;
	//Tonemapping with reinhart operator
	color = color / (color + vec3(1.0));
	FragColor = vec4(color, 1.0);
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

vec3 CalcDirLight(DirLight light, vec3 N, vec3 V, vec3 albedo) {

	vec3 L = normalize(light.direction);
	vec3 H = normalize(V + L);
	vec3 radiance = light.color;
	//For non metallic surface, F0 for fresnel is always 0.04. Otherwise we mix
	//with albedo to tint the reflectance.
	vec3 F0 = vec3(0.04, 0.04, 0.04);
	F0 = mix(F0, albedo, material.metallic);
	//Calculating the various components of the rendering equation
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
	float roughness = 0.0;
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	//Actually using those components
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.001);
	//F directly corresponds to Ks and for the law of energy conservation, we
	//can easily find Kd. We than weight this value with the metallic value
	//(no diffuse if fully metallic)
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.metallic;
	//Summing the contribution of the light
	float NdotL = max(dot(N, L), 0.0);
	vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
	return Lo;
}



