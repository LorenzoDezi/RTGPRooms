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
	//TODO: choose right alpha value and alpha multiplier
	grassShader.setFloat("alphaTest", 0.01f);
	grassShader.setFloat("alphaMultiplier", 0.2f);
}

void Grass::Draw(glm::vec3 position, glm::mat4 view, glm::mat4 projection, float time)
{
	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	grassShader.use();
	grassShader.setMat4Float("projMatrix", glm::value_ptr(projection));
	grassShader.setMat4Float("viewMatrix", glm::value_ptr(view));
	grassShader.setMat4Float("modelMatrix", glm::value_ptr(model));
	grassShader.setFloat("time", time);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	grassShader.setInt("texture", 0);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void Grass::DrawInstanced(glm::mat4 view, glm::mat4 projection, float time)
{
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
