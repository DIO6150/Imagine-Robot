#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>

#include <Robot/Common/Tile.hpp>

#include <Robot/Render/ShaderProgram.hpp>

#include <Robot/Render/SharedResourcesArray.hpp>

class MapRenderer
{
public:
	void init(SharedResources * resources);
	
	void setMap(std::vector<Tile> const & map, uint32_t width, uint32_t height);
	void setAgentPosition(Pos2D pos);
	void setAgentPosition(glm::vec2 pos);
	void setAgentColor(glm::vec3 color);

	void draw();

	glm::vec2 from(Pos2D tilePos) const;
	
private:
	struct Sprite
	{
		glm::vec3 albedoColor = {1.0, 1.0, 1.0};
		GLuint albedoTexture  = 0;
		glm::mat4 transform;
	};

	std::vector<Sprite> tileList_;

	Sprite agent_;

private:
	uint32_t mapWidth_  = 0;
	uint32_t mapHeight_ = 0;
	glm::mat4 mapTransform_;
	float scale_;

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

	SharedResources * resources_;

	std::map<TextureType, GLuint> textures_;
};