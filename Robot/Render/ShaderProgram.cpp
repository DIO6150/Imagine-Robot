
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <Robot/Render/ShaderProgram.hpp>

#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>

ShaderProgram::ShaderProgram()
{
	
}

void ShaderProgram::compileVertex(std::string source)
{
	if (source == "") return;

	if (vertexShader_ != 0)
	{
		std::cout << "Error : Recompiling Vertex Shader : " << program_ << "\n";
		glDeleteShader(vertexShader_);
	}

	vertexShader_ = glCreateShader(GL_VERTEX_SHADER);

	char const * vertexCode = source.c_str ();
	glShaderSource(vertexShader_, 1, &vertexCode, NULL);
	glCompileShader(vertexShader_);

	int status;
	char infoLog[512];
	glGetShaderiv(vertexShader_, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		glGetShaderInfoLog(vertexShader_, 512, NULL, infoLog);
		std::cout << "Error : Compiling Vertex Shader : " << infoLog << "\n";
	}
}

void ShaderProgram::compileFragment(std::string source)
{
	if (source == "") return;

	if (fragmentShader_ != 0)
	{
		std::cout << "Error : Recompiling Fragment Shader : " << program_ << "\n";
		glDeleteShader(fragmentShader_);
	}

	fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);

	char const * fragmentCode = source.c_str ();
	glShaderSource(fragmentShader_, 1, &fragmentCode, NULL);
	glCompileShader(fragmentShader_);

	int status;
	char infoLog[512];
	glGetShaderiv(fragmentShader_, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		glGetShaderInfoLog(fragmentShader_, 512, NULL, infoLog);
		std::cout << "Error : Compiling Fragment Shader : " << infoLog << "\n";
	}
}

void ShaderProgram::link()
{
	if (program_)
	{
		glDeleteProgram (program_);
		std::cout << "Error : Recompiling Shader PRogram : " << program_ << "\n";
	}

	program_ = glCreateProgram();
	
	glAttachShader(program_, vertexShader_);
	glAttachShader(program_, fragmentShader_);
	glLinkProgram(program_);
	
	int status;
	char infoLog[512];
	glGetProgramiv(program_, GL_LINK_STATUS, &status);
	if(!status)
	{
		glGetProgramInfoLog(program_, 512, NULL, infoLog);
		std::cout << "Error : Linking Shader Program : " << infoLog << "\n";
		program_ = 0;
	}
}

void ShaderProgram::use() const
{
	glUseProgram(program_);
}

void ShaderProgram::cancel() const
{
	glUseProgram(0);
}

// TODO-add : create a cache for uniform locations
void ShaderProgram::uploadMatrix4(std::string key, const glm::mat4& matrix) const
{
    int location;

    location = glGetUniformLocation(program_, key.c_str ());
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::upload1i(std::string key, int i) const
{
    int location;

    location = glGetUniformLocation(program_, key.c_str ());
    glUniform1i(location, i);
}

void ShaderProgram::upload3fv(std::string key, glm::vec3 vec) const
{
    int location;

    location = glGetUniformLocation(program_, key.c_str ());
    glUniform3fv(location, 1, &vec.x);
}

void ShaderProgram::upload1f(std::string key, float f) const
{
    int location;

    location = glGetUniformLocation(program_, key.c_str ());
    glUniform1f(location, f);
}
