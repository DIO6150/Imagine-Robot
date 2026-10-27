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
	void setAgentPosition(Pos2D pos);
	void setAgentPosition(glm::vec2 pos);
	void setAgentColor(glm::vec3 color);

	void draw();
	
private:
	struct MapTile
	{
		glm::vec3 albedoColor = {1.0, 1.0, 1.0};
		GLuint albedoTexture  = 0;
		glm::vec2 position = {0.0, 0.0};
	};

	std::vector<MapTile> tileList_;

	MapTile agent_;

private:
	uint32_t mapWidth_  = 0;
	uint32_t mapHeight_ = 0;

	enum TextureType
	{
		Custommer,
		Empty,
		Dictionary,
		Wall,
		Blank,
		Agent,
		Chest,
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