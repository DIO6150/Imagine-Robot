#pragma once

#include <Robot/Render/Mesh.hpp>

#include <glad/glad.h>

struct Rect : Mesh
{
	void initBuffers() override
	{
		glGenVertexArrays(1, &vao_);
		glGenBuffers(1, &vbo_);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		std::vector<Vertex> vertices = {
			{ 0.0f, 0.0f, 0.0f,  0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f,  1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f,  1.0f, 1.0f },

			{ 0.0f, 0.0f, 0.0f,  0.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f,  1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f,  0.0f, 1.0f }
		};

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	}

	void draw() const override
	{
		glBindVertexArray(vao_); // fine because we draw so little
		program_.use();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		program_.cancel();
	}
};
