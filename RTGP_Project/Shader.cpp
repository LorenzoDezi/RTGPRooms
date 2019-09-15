#include "Shader.h"

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath, const GLchar * geometryPath,
	const GLchar *tControlPath, const GLchar *tEvaluationPath)
{
	//Extracting and compiling shader sources
	std::string vertexCode = extractShaderFromFile(vertexPath);
	std::string fragCode = extractShaderFromFile(fragmentPath);
	GLuint vertexID, fragID, geometryID, tControlID, tEvaluationID;
	vertexID = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
	fragID = compileShader(fragCode.c_str(), GL_FRAGMENT_SHADER);
	if (geometryPath != nullptr) {
		std::string geometryCode = extractShaderFromFile(geometryPath);
		geometryID = compileShader(geometryCode.c_str(), GL_GEOMETRY_SHADER);
	}
	if (tControlPath != nullptr) {
		std::string tControlCode = extractShaderFromFile(tControlPath);
		tControlID = compileShader(tControlCode.c_str(), GL_TESS_CONTROL_SHADER);
	}
	if (tEvaluationPath != nullptr) {
		std::string tEvaluationCode = extractShaderFromFile(tEvaluationPath);
		tEvaluationID = compileShader(tEvaluationCode.c_str(), GL_TESS_EVALUATION_SHADER);
	}

	//Creating and linking program
	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertexID);
	glAttachShader(this->ID, fragID);
	if (geometryPath != nullptr)
		glAttachShader(this->ID, geometryID);
	if (tControlPath != nullptr)
		glAttachShader(this->ID, tControlID);
	if (tEvaluationPath != nullptr)
		glAttachShader(this->ID, tEvaluationID);
	glLinkProgram(this->ID);
	checkProgramLinking(this->ID);

	//Delete unnecessary shaders
	glDeleteShader(vertexID);
	glDeleteShader(fragID);
	if(geometryPath != nullptr)
		glDeleteShader(geometryID);
	if (tControlPath != nullptr)
		glDeleteShader(tControlID);
	if (tEvaluationPath != nullptr)
		glDeleteShader(tEvaluationID);
}

std::string Shader::extractShaderFromFile(const GLchar *shaderPath) const {
    std::string vertexCode;
    std::ifstream vShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(shaderPath);
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vShaderFile.close();
        vertexCode = vShaderStream.str();

    } catch (std::ifstream::failure e) {
        std::cout << "ERROR: SHADER FILE NOT SUCCESSFULLY READ!" << std::endl;
    }
    return vertexCode;
}

void Shader::use() {
    glUseProgram(this->ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::checkShaderCompilation(unsigned int vertexShader) {
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void Shader::checkProgramLinking(unsigned int shaderProgram) {
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

GLuint Shader::compileShader(const char *code, GLenum type) {
    GLuint shaderID;
    shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &code, NULL);
    glCompileShader(shaderID);
    checkShaderCompilation(shaderID);
    return shaderID;
}

void Shader::setVec4Float(const std::string &name, float v1, float v2, float v3, float v4) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()),v1, v2, v3, v4);
}

void Shader::setVec3Float(const std::string & name, float v1, float v2, float v3) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3);
}

void Shader::setMat4Float(const std::string & name, const float * mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

