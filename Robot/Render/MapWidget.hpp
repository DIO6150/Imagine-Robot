#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>

#include <Robot/Common/Tile.hpp>

#include <Robot/Render/ShaderProgram.hpp>
#include <Robot/Render/Widget.hpp>

class MapWidget: public AbstractWidget
{
public:
	MapWidget(SpriteRenderer & renderer, SharedResourceArray & resources);
	void initialize(
		std::vector<Tile> const & map,
		uint32_t width,
		uint32_t height,
		Pos2D agentPosition,
		uint32_t windowWidth,
		uint32_t windowHeight
	);

	void setAgentPosition(glm::vec3 pos);
	void setAgentColor(glm::vec3 color);
	void setAgentCarry(bool value);

	glm::vec3 getAgentPosition() const;
	glm::vec3 getTilePosition(Pos2D tilePos) const;
	glm::vec3 getTileScale() const;

	void openChest();
	void closeChest();

	void onRender() override;

	NeededResources onResourcePreLoad() override
	{
		NeededResources needed;
		needed.meshes = {

		};

		needed.textures = {
			"empty"      ,
			"wall"       ,
			"chest"      ,
			"chest_open" ,
			"guy"        ,
			"guy_carry"  ,
			"dictionary" ,
		};

		needed.shaders = {

		};

		return needed;
	}
	
private:
	float targetSize_ = 1.5f;

	glm::vec3 mapPos_;

	uint32_t mapWidth_;
	uint32_t mapHeight_;

	uint32_t windowWidth_;
	uint32_t windowHeight_;

	float tileScale_;

	Sprite agent_;
	std::vector<Sprite> tiles_;
	uint32_t chestIndex_;

	glm::mat4 mapTransform_;
};