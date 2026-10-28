#pragma once

#include <Robot/Render/ShaderProgram.hpp>
#include <Robot/Render/Shaders/TileVertexShader.hpp>
#include <Robot/Render/Shaders/TileFragmentShader.hpp>
#include <Robot/Render/Vertex.hpp>

#include <glad/glad.h>

struct SharedResources
{
	GLuint rectVao;
	GLuint rectVbo;
	ShaderProgram rectProgram;

	void init()
	{
		glGenVertexArrays(1, &rectVao);
		glBindVertexArray(rectVao);

		glGenBuffers(1, &rectVbo);

		glBindBuffer(GL_ARRAY_BUFFER, rectVbo);

		std::vector<Vertex> vertices = {
			{ 0.0f, 0.0f, 0.0f,  0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f,  1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f,  1.0f, 1.0f },

			{ 0.0f, 0.0f, 0.0f,  0.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f,  1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f,  0.0f, 1.0f }
		};

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			false,
			5 * sizeof(float),
			(void *) 0
		);

		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			false,
			5 * sizeof(float),
			(void *) (3 * sizeof (float))
		);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		rectProgram.compileVertex(TILE_VERTEX_SHADER);
		rectProgram.compileFragment(TILE_FRAGMENT_SHADER);
		rectProgram.link();
	}
};