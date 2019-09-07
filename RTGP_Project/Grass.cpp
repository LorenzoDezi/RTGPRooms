#include "Grass.h"



Grass::Grass() : grassShader(
	"Shaders/vertex_grass.glsl", "Shaders/fragment_grass.glsl", "Shaders/geometry_grass.glsl")
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
	initTextures();
	grassShader.setFloat("alphaTest", 0.01f);
	grassShader.setFloat("alphaMultiplier", 0.2f);
}

void Grass::setPositions(const std::vector<glm::vec3> &positions)
{
	srand(time(NULL));
	//Creating model matrices
	std::vector<GrassInput> grassInputs;
	for (auto& pos : positions) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pos);
		GLint index = rand() % 9 + 0;
		grassInputs.push_back(
			GrassInput{
				model, index,
			}
		);
	}

	amount = grassInputs.size();
	glGenBuffers(1, &instancingVBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, instancingVBO);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(GrassInput), &grassInputs[0], GL_STATIC_DRAW);
	GLsizei vec4Size = sizeof(glm::vec4);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GrassInput), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GrassInput), (void*)(vec4Size));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GrassInput), (void*)(2 * vec4Size));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GrassInput), (void*)(3 * vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 1, GL_INT, sizeof(GrassInput),
		(void *) (4 * vec4Size));

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
}

void Grass::Draw(const glm::mat4 &view, const glm::mat4 &projection, float time)
{
	grassShader.use();
	grassShader.setMat4Float("projMatrix", glm::value_ptr(projection));
	grassShader.setMat4Float("viewMatrix", glm::value_ptr(view));
	grassShader.setFloat("time", time);
	for (int i = 0; i < 9; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, grassTextures[i]);
	}
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_POINTS, 0, 1, amount);
	glBindVertexArray(0);
}



Grass::~Grass()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &instancingVBO);
	glDeleteTextures(NR_GRASS_TEXTURES, &grassTextures[0]);
}

void Grass::initTextures()
{
	std::string baseName = "Grass_Blade_";
	grassShader.use();
	for (int i = 0; i < NR_GRASS_TEXTURES; i++) {
		grassTextures[i] = ImageUtility::TextureFromFile((baseName + std::to_string(i) + ".png").c_str(), "Assets/textures/grass");
		grassShader.setInt("texture" + std::to_string(i), i);
	}
}
