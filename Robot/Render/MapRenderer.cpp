
#include <Robot/Render/MapRenderer.hpp>
#include <Robot/Render/Vertex.hpp>

#include <Robot/Render/Shaders/TileVertexShader.hpp>
#include <Robot/Render/Shaders/TileFragmentShader.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb/stb_image.h>

#include <iostream>

struct Image
{
	std::vector<uint8_t> pixels;
	int32_t width;
	int32_t height;
	int32_t channels;

	void load(std::string path)
	{
		auto * px = stbi_load(path.c_str(), &width, &height, &channels, 4);
		pixels.clear();
		
		if (!px)
		{
			width    = 0;
			height   = 0;
			channels = 0;
			std::cout << "Error: couldnt load texture : " << path << "\n";
			return;
		}
		
		auto pixelCount = width * height * channels;
		pixels.reserve(pixelCount);

		pixels.assign(px, px + pixelCount);
		stbi_image_free(px);
	}

	void loadSinglePixelTexture(glm::vec3 color)
	{
		pixels.clear();

		uint8_t r = 255 * color.r;
		uint8_t g = 255 * color.g;
		uint8_t b = 255 * color.b;

		pixels.push_back(r);
		pixels.push_back(g);
		pixels.push_back(b);
		pixels.push_back(255);

		width    = 1;
		height   = 1;
		channels = 4;
	}

	GLuint createTexture()
	{
		if (pixels.empty())
			return 0;

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
		glGenerateMipmap(GL_TEXTURE_2D);

		return texture;
	}
};

void MapRenderer::init()
{
	glGenVertexArrays(1, &tileVao_);
	glBindVertexArray(tileVao_);

	glGenBuffers(1, &tileVbo_);

	glBindBuffer(GL_ARRAY_BUFFER, tileVbo_);

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

	tileProgram_.compileVertex(TILE_VERTEX_SHADER);
	tileProgram_.compileFragment(TILE_FRAGMENT_SHADER);
	tileProgram_.link();

	stbi_set_flip_vertically_on_load(true);

	Image blank, wall, dictionnary, empty, custommer, chest;
	
	custommer  .load("data/assets/textures/guy.png");
	dictionnary.load("data/assets/textures/dictionnary.png");
	empty      .load("data/assets/textures/empty.png");
	wall       .load("data/assets/textures/wall.png");
	chest      .load("data/assets/textures/chest.png");
	blank      .loadSinglePixelTexture({1.f, 1.f, 1.f});

	textures_[Custommer]  = custommer  .createTexture();
	textures_[Dictionary] = dictionnary.createTexture();
	textures_[Empty]      = empty      .createTexture();
	textures_[Wall]       = wall       .createTexture();
	textures_[Blank]      = blank      .createTexture();
	textures_[Agent]      = textures_[Custommer];
	textures_[Chest]      = chest      .createTexture();

	agent_.albedoTexture = textures_[Agent];
	agent_.albedoColor   = {1.0, 0.0, 0.0};
}

void MapRenderer::setMap(std::vector<Tile> const & map, uint32_t width, uint32_t height)
{
	mapWidth_  = width;
	mapHeight_ = height;

	tileList_.clear();
	tileList_.reserve(map.size());
	
	for (auto tile : map)
	{
		MapTile obj;
		
		obj.position.x = tile.pos_.c;
		obj.position.y = (height - 1) - tile.pos_.l;

		obj.albedoTexture = textures_[Blank];

		if (tile.isEmpty())
			obj.albedoTexture = textures_[Empty];
		else if (tile.isPerson())
		{
			obj.albedoColor   = {0.f, 0.f, 1.f};
			obj.albedoTexture = textures_[Custommer];
		}
		else if (tile.isRecord())
			obj.albedoTexture = textures_[Dictionary];
		else if (tile.isAgentStart())
		{
			obj.albedoTexture = textures_[Empty];
			agent_.position = obj.position;
		}
		else if (tile.isItemPickup())
			obj.albedoTexture = textures_[Chest];
		else if (tile.isSolid())
			obj.albedoTexture = textures_[Wall];

		if (tile.isPerson())
		{
			MapTile floor;
			floor.position = obj.position;
			floor.albedoTexture = textures_[Empty];
			tileList_.push_back(floor);
		}
		tileList_.push_back(obj);
	}

	const float targetSize = 1.5f;
	const float scale = targetSize / std::max(width, height);

	mapTransform_ = glm::mat4{1.0f};

	mapTransform_ = glm::translate(
		mapTransform_,
		glm::vec3
		{
			-scale * width  * .5f,
			-scale * height * .5f,
			.0f
		}
	);

	mapTransform_ = glm::scale(
		mapTransform_,
		{scale, scale, 1.0f}
	);
}

void MapRenderer::setAgentPosition(Pos2D newPos)
{
	agent_.position.x = newPos.c;
	agent_.position.y = (mapHeight_ - 1) - newPos.l;
}

void MapRenderer::setAgentPosition(glm::vec2 pos)
{
	agent_.position.x = pos.x;
	agent_.position.y = static_cast<float>(mapHeight_ - 1) - pos.y;
}

void MapRenderer::setAgentColor(glm::vec3 color)
{
	agent_.albedoColor = color;
}

void MapRenderer::draw()
{
	// draw grid here
	//glBindVertexArray(gridVao_);

	// draw map data here

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(tileVao_);
	tileProgram_.use();
	for (auto tile : tileList_)
	{
		//glm::vec3 tileTranslation = glm::vec4 {tile.position, 0.0f, 0.0f} * mapTransform_;
		//glm::mat4 transform { 1.0f };
		//transform = glm::translate(transform, tileTranslation);
		//transform = glm::translate(transform, glm::vec3{-0.5f, -0.5f, 0.0f});

		glm::mat4 transform = glm::translate(mapTransform_, glm::vec3 {tile.position, 0.0f});
		tileProgram_.uploadMatrix4("u_transform", transform);

		if (tile.albedoTexture)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tile.albedoTexture);
		}

		tileProgram_.upload3fv("u_color", tile.albedoColor);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glm::mat4 transform = glm::translate(mapTransform_, glm::vec3 {agent_.position, 0.0f});
	tileProgram_.uploadMatrix4("u_transform", transform);
	if (agent_.albedoTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, agent_.albedoTexture);
	}
	tileProgram_.upload3fv("u_color", agent_.albedoColor);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
}
