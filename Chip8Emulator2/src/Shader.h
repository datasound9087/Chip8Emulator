#pragma once

#include <glad/glad.h>
#include <string>

class Shader
{
public:
	Shader(std::string path, std::string name);
	~Shader();

	void use();

private:

	GLuint mID;

	void checkErrors(GLuint shader, std::string type);
};

