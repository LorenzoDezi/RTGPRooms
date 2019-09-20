#version 430 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;


struct Material {
	sampler2D texture_diffuse;
	float shininess;
	float Ka;
	float Kd;
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
in vec2 TexCoords;

uniform vec3 objectColor;
uniform Material material;

#define NR_POINT_LIGHTS 6
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;

uniform float textureScale;


vec3 CalcDirLight(DirLight light, vec3 norm, vec2 texCoords);
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec2 texCoords);


void main() {
	vec2 texCoords = vec2(TexCoords.x * textureScale, TexCoords.y * textureScale);
	vec3 norm = normalize(Normal);
	vec3 result = CalcDirLight(dirLight, norm, texCoords);
	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		result += CalcPointLight(pointLights[i], norm, FragPos, texCoords);
	}
	FragColor = vec4(result, 1.0);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness >= 3.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

}

vec3 CalcDirLight(DirLight light, vec3 norm, vec2 texCoords) {
	vec3 lightDir = normalize(-light.direction);
	//Ambient calculation
	vec3 ambient = vec3(texture(material.texture_diffuse, texCoords)) * light.ambient * material.Ka;
	//diffuse calculation
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(material.texture_diffuse, texCoords))) * light.diffuse * material.Kd;
	//For the torch, we haven't the specular texture so we must skip for the specular calculation
	//directional light contribution
	return diffuse + ambient;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos,vec2 texCoords) {
	vec3 lightDir = normalize(light.position - fragPos);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	//Ambient calculation
	vec3 ambient = vec3(texture(material.texture_diffuse, texCoords)) * light.ambient * attenuation * material.Ka;
	//diffuse calculation
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(material.texture_diffuse, texCoords))) * light.diffuse * attenuation * material.Kd;
	//For the torch, we haven't the specular texture so we must skip the specular calculation
	//point light contribution
	return diffuse + ambient;
}

