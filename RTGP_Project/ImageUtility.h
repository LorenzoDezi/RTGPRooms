#pragma once
#include "stb_image/stb_image.h"
#include <glad/glad.h>
#include <iostream>
#include <string>


class ImageUtility
{
public:
	GLuint static TextureFromFile(const char *path, const std::string &directory);
	~ImageUtility();

private:
	ImageUtility();
};

