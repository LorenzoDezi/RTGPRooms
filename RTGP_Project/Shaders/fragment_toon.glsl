#version 430 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;


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

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in mat3 TBN;
in vec2 TexCoords;

uniform vec3 objectColor;
uniform vec3 viewPos;
uniform Material material;

#define NR_POINT_LIGHTS 6
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;

uniform float textureScale;

void main() {
	FragColor = vec4(material.color_diffuse, 1.0);
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness >= 3.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

}


