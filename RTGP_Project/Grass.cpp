#include "Grass.h"



Grass::Grass() : grassShader("Shaders/vertex_grass.glsl", "Shaders/fragment_grass.glsl", "Shaders/geometry_grass.glsl")
{
	//Vertex setup
	float vertex[] = { 0.0f, 0.0f, 0.0f };
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), &vertex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	//Texture setup
	grassTexture = TextureFromFile("Grass_Blade_02.png", "Assets/textures/grass", false);
	grassShader.use();
	grassShader.setInt("texture", 0);
	grassShader.setFloat("alphaTest", 0.01f);
	grassShader.setFloat("alphaMultiplier", 0.2f);
}

void Grass::setPositions(std::vector<glm::vec3> positions)
{
	//Creating model matrices
	std::vector<glm::mat4> modelMatrices;
	for (auto& pos : positions) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		modelMatrices.push_back(model);
	}
	amount = modelMatrices.size();
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	GLsizei vec4Size = sizeof(glm::vec4);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glBindVertexArray(0);
}

void Grass::Draw(glm::mat4 view, glm::mat4 projection, float time)
{
	grassShader.use();
	grassShader.setMat4Float("projMatrix", glm::value_ptr(projection));
	grassShader.setMat4Float("viewMatrix", glm::value_ptr(view));
	grassShader.setFloat("time", time);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glDrawArraysInstanced(GL_POINTS, 0, 1, amount);
	glBindVertexArray(0);
}

//TODO: Find a unique place for this function
//TODO: Also delete the gamma value
GLuint Grass::TextureFromFile(const char * path, const std::string & directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}



Grass::~Grass()
{
}
