
#include <Robot/Render/SharedResourcesArray.hpp>

void SharedResourceArray::createMesh(std::string key, std::vector<float> const & vertices, uint64_t layout)
{
	if (getMesh(key)) return;

	meshes_.emplace(
		std::piecewise_construct, 
		std::forward_as_tuple(key),
		std::forward_as_tuple(vertices, layout, getVertexSize(layout))
	);
}

std::optional<Mesh> SharedResourceArray::getMesh(std::string key)
{
	auto it = meshes_.find(key);

	if (it == meshes_.end())
		return std::nullopt;
	
	return it->second;
}

void SharedResourceArray::bindMesh(std::string key)
{
	auto mesh = getMesh(key);
	if (!mesh) return;
	glBindVertexArray(mesh.value().vao);
}


void SharedResourceArray::createTexture(std::string key, Image const & image)
{
	if (getTexture(key)) return;
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	textures_.emplace(key, texture);
}

std::optional<GLuint> SharedResourceArray::getTexture(std::string key)
{
	auto it = textures_.find(key);

	if (it == textures_.end())
		return std::nullopt;

	return it->second;
}

void SharedResourceArray::bindTexture(std::string key, uint32_t slot)
{
	auto texture = getTexture(key);
	if (!texture) return;

	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture.value());
}

void SharedResourceArray::createProgram(std::string key, std::string vertexSource, std::string fragmentSource)
{
	if (getProgram(key)) return;

	ShaderProgram program;
	program.compileVertex(vertexSource);
	program.compileFragment(fragmentSource);
	program.link();

	programs_.emplace(key, std::move(program));
}

std::optional<ShaderProgram> SharedResourceArray::getProgram(std::string key)
{
	auto it = programs_.find(key);

	if (it == programs_.end())
		return std::nullopt;

	return it->second;
}



Mesh::Mesh(std::vector<float> vertices, uint64_t layout, uint32_t vertexSize)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	int currentAttibIndex = 0;
	uint32_t offset = 0;
	if (layout & SharedResourceArray::Vertex)
	{
		glVertexAttribPointer(
			currentAttibIndex,
			3,
			GL_FLOAT,
			false,
			vertexSize * sizeof(float),
			(void *) offset
		);
		glEnableVertexAttribArray(currentAttibIndex++);
		offset += 3 * sizeof(float);
	}
	if (layout & SharedResourceArray::Normal)
	{
		glVertexAttribPointer(
			currentAttibIndex,
			3,
			GL_FLOAT,
			false,
			vertexSize * sizeof(float),
			(void *) offset
		);
		glEnableVertexAttribArray(currentAttibIndex++);
		offset += 3 * sizeof(float);
	}
	if (layout & SharedResourceArray::UV)
	{
		glVertexAttribPointer(
			currentAttibIndex,
			2,
			GL_FLOAT,
			false,
			vertexSize * sizeof(float),
			(void *) offset
		);
		glEnableVertexAttribArray(currentAttibIndex++);
		offset += 2 * sizeof(float);
	}

	vertexCount = vertices.size() / vertexSize;
}

uint32_t SharedResourceArray::getVertexSize(uint32_t layout)
{
	uint32_t size = 0;

	if (layout & Vertex) size += 3;
	if (layout & Normal) size += 3;
	if (layout & UV)     size += 2;

	return size;
}