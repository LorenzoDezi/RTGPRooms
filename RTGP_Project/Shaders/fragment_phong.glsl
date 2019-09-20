#version 430 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;


struct Material {
	sampler2D texture_diffuse;
	sampler2D texture_specular;
	sampler2D texture_normals;
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
in mat3 TBN;
in vec2 TexCoords;

uniform vec3 objectColor;
uniform vec3 viewPos;
uniform Material material;

#define NR_POINT_LIGHTS 6
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;

uniform float textureScale;


vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec2 texCoords);
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec2 texCoords);


void main() {
	vec2 texCoords = vec2(TexCoords.x * textureScale, TexCoords.y * textureScale);
	vec3 norm = vec3(texture(material.texture_normals, texCoords));
	norm = normalize(norm * 2.0 - 1.0);
	norm = normalize(TBN * norm);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = CalcDirLight(dirLight, norm, viewDir, texCoords);
	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, texCoords);
	}	
	FragColor = vec4(result, 1.0);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness >= 3.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

}

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec2 texCoords) {
	vec3 lightDir = normalize(-light.direction);
	//Ambient calculation
	vec3 ambient = vec3(texture(material.texture_diffuse, texCoords)) * light.ambient * material.Ka;
	//diffuse calculation
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(material.texture_diffuse, texCoords))) * light.diffuse * material.Kd;
	//specular calculation
	vec3 halfwayDir = normalize(lightDir + viewDir);	
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular, TexCoords)) * material.Ks;
	//directional light contribution
	return specular + diffuse + ambient;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec2 texCoords) {
	vec3 lightDir = normalize(light.position - fragPos);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	//Ambient calculation
	vec3 ambient = vec3(texture(material.texture_diffuse, texCoords)) * light.ambient * attenuation * material.Ka;
	//diffuse calculation
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(material.texture_diffuse, texCoords))) * light.diffuse * attenuation * material.Kd;
	//specular calculation
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//The specular map is saved in grayscale, but without this its value will be read as GL_RED
	vec3 specularTexture = vec3(texture(material.texture_specular, texCoords));
	vec3 specular = light.specular * spec * vec3(specularTexture.r, specularTexture.r, specularTexture.r) * attenuation * material.Ks;
	//point light contribution
	return specular + diffuse + ambient;
}

