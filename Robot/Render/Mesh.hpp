#pragma once

#include <Robot/Render/ShaderProgram.hpp>

using handle_t = uint32_t;

struct Vertex
{
	float x, y, z;
	float u, v;
};

struct Mesh
{
	handle_t vao_;
	handle_t vbo_; // for drawing squares a single vbo is more than enough
	ShaderProgram program_;

	virtual void initBuffers() {}
	virtual void draw() const = 0;

	void setShader(std::string vertex, std::string fragment)
	{
		program_.compileVertex  (vertex);
		program_.compileFragment(fragment);
		program_.link();
	}

};