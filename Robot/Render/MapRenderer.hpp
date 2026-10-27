#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>

#include <Robot/Common/Tile.hpp>

#include <Robot/Render/ShaderProgram.hpp>

class MapRenderer
{
public:
	void init();
	
	void setMap(std::vector<Tile> const & map, uint32_t width, uint32_t height);

	void draw();
	
private:
	struct MapTile
	{
		glm::vec3 albedoColor = {1.0, 1.0, 1.0};
		GLuint albedoTexture  = 0;
		glm::vec2 position = {0.0, 0.0};
	};

	std::vector<MapTile> tileList_;

private:
	enum TextureType
	{
		Custommer,
		Empty,
		Dictionary,
		Wall,
		Blank
	};

	GLuint gridVao_;
	GLuint gridVbo_;

	/* TILES */
	GLuint tileVao_;
	GLuint tileVbo_;
	ShaderProgram tileProgram_;
	glm::mat4 mapTransform_;

	std::map<TextureType, GLuint> textures_;
};