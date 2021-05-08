#include "Shader.h"

#include <sstream>
#include <fstream>
#include <iostream>

Shader::Shader(std::string path, std::string name)
{
	using namespace std;

	string vCode;
	string fCode;
	ifstream vFile;
	ifstream fFile;

	vFile.exceptions(ifstream::failbit | ifstream::badbit);
	fFile.exceptions(ifstream::failbit | ifstream::badbit);

	try
	{
		vFile.open(path + "/" + name + ".vert");
		fFile.open(path + "/" + name + ".frag");

		stringstream vStream, fStream;
		vStream << vFile.rdbuf();
		fStream << fFile.rdbuf();

		vFile.close();
		fFile.close();

		vCode = vStream.str();
		fCode = fStream.str();

	}catch(ifstream::failure e)
	{
		cout << "ERROR: SHADER FILE NOT READ SUCESSFULLY: " << path << "/<" << name << ">.<ext>" << endl;
	}

	const char* cVCode = vCode.c_str();
	const char* cFCode = fCode.c_str();


	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &cVCode, NULL);
	glCompileShader(vertex);
	checkErrors(vertex, "VERTEX");


	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &cFCode, NULL);
	glCompileShader(fragment);
	checkErrors(fragment, "FRAGMENT");

	mID = glCreateProgram();
	glAttachShader(mID, vertex);
	glAttachShader(mID, fragment);
	glLinkProgram(mID);
	checkErrors(mID, "LINK");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(mID);
}

void Shader::use()
{
	glUseProgram(mID);
}

void Shader::checkErrors(GLuint shader, std::string type)
{
	int success;
	char infolog[1024];

	if(type == "LINK")
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		
		if(!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infolog);
			std::cout << "ERROR: PROG LINK ERROR of type: " << type << "\n" << infolog << "\n" << std::endl;
		}

	}else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		
		if(!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infolog);
			std::cout << "ERROR: SHADER LINK ERROR of type: " << type << "\n" << infolog << "\n" << std::endl;
		}
	}
}
