#pragma once

#include <Robot/Render/Image.hpp>
#include <Robot/Render/ShaderProgram.hpp>
#include <Robot/Render/Shaders/TileVertexShader.hpp>
#include <Robot/Render/Shaders/TileFragmentShader.hpp>
#include <Robot/Render/Vertex.hpp>

#include <glad/glad.h>

#include <map>
#include <optional>

struct Mesh
{
	GLuint vao;
	GLuint vbo;

	uint32_t vertexCount;

	Mesh (std::vector<float> vertices, uint64_t layout, uint32_t vertexSize);
};

class SharedResourceArray
{
public:
	enum VertexLayout : uint64_t
	{
		Vertex = 1 << 1,
		Normal = 1 << 2,
		UV     = 1 << 3,
	};

	void createMesh(std::string key, std::vector<float> const & vertices, uint64_t layout);
	std::optional<Mesh> getMesh(std::string key);
	std::optional<uint32_t> getMeshVertexCount(std::string key);
	void bindMesh(std::string key);

	void createTexture(std::string key, Image const & image);
	std::optional<GLuint> getTexture(std::string key);
	void bindTexture(std::string key, uint32_t slot);

	void createProgram(std::string key, std::string vertexSource, std::string fragmentSource);
	std::optional<ShaderProgram> getProgram(std::string key);

private:
	uint32_t getVertexSize(uint32_t layout);

private:
	std::map<std::string, Mesh> meshes_;
	std::map<std::string, GLuint> textures_;
	std::map<std::string, ShaderProgram> programs_;
};
