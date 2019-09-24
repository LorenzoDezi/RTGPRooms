#version 430 core
layout(location = 0) out vec4 FragColor;

struct Material {
	vec3 color_diffuse;
	float shininess;
	float Kd;
	float Ka;
	float Ks;
};

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;

#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;

const int levels = 10;
const float scaleFactor = 1.0 / levels;

float CalcPointLight(vec3 norm, vec3 V, PointLight pointLight);

void main() {
	float lightContribution = 0.0;
	vec3 norm = normalize(Normal);
	vec3 V = normalize(viewPos - FragPos);
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
		lightContribution += CalcPointLight(norm, V, pointLights[i]);
	vec3 color = material.color_diffuse * lightContribution;
	FragColor = vec4(color, 1.0);
}

float CalcPointLight(vec3 norm, vec3 V, PointLight pointLight) {
	vec3 L = normalize(pointLight.position - FragPos);
	vec3 H = normalize(L + V);
	float diffuse = max(0, dot(L, norm));
	diffuse = material.Kd * floor(levels * diffuse) * scaleFactor;
	float specular = 0.0;
	if (dot(L, norm) > 0.0) {
		specular = material.Kd * pow(max(0, dot(H, norm)), material.shininess);
	}
	return diffuse + specular;
}


