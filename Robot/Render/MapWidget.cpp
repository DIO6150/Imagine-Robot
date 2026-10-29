
#include <Robot/Render/MapWidget.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb/stb_image.h>

#include <iostream>

#include <Robot/Render/Image.hpp>

MapWidget::MapWidget(SpriteRenderer & renderer, SharedResourceArray & resources)
	: AbstractWidget {renderer, resources}
{
	
}

void MapWidget::initialize(
	std::vector<Tile> const & map,
	uint32_t width,
	uint32_t height,
	Pos2D agentPosition,
	uint32_t windowWidth,
	uint32_t windowHeight
)
{
	mapWidth_  = width;
	mapHeight_ = height;

	windowWidth_  = windowWidth;
	windowHeight_ = windowHeight;

	chestIndex_ = -1;
	tiles_.clear();
	tiles_.reserve(map.size());
	
	tileScale_ = (targetSize_ * std::max(windowWidth_, windowHeight_) * .5f) / std::max(width, height);
	
	auto result = renderer_.createSprite("default", "default", "default");
	if (!result)
	{
		std::cout << "Error : couldnt create MapWidget\n";
		return;
	}
	
	for (auto tile : map)
	{
		auto tileSprite = result.value();
		tileSprite.position = getTilePosition(tile.pos_);
		tileSprite.scale    = getTileScale();

		if (tile.isEmpty())
			tileSprite.texture = getTexture("empty");
		else if (tile.isPerson())
		{
			tileSprite.texture = getTexture("empty");
			tileSprite.updateTransform();
			tiles_.push_back(tileSprite);
			tileSprite.color   = {0.f, 0.f, 1.f};
			tileSprite.texture = getTexture("guy");
			tileSprite.position.z = 0.1f;
		}
		else if (tile.isRecord())
			tileSprite.texture = getTexture("dictionary");
		else if (tile.isAgentStart())
		{
			tileSprite.texture = getTexture("empty");
			agent_.transform = tileSprite.transform;
		}
		else if (tile.isItemPickup())
		{
			tileSprite.texture = getTexture("chest");
			chestIndex_ = tiles_.size();
		}
		else if (tile.isSolid())
			tileSprite.texture = getTexture("wall");

		tileSprite.updateTransform();
		tiles_.push_back(tileSprite);
	}

	agent_ = result.value();
	agent_.position = getTilePosition(agentPosition);
	agent_.position.z = 0.1f;
	agent_.scale    = getTileScale();
	agent_.color    = {1., .0, .0};
	setAgentCarry(false);

	agent_.updateTransform();
}

void MapWidget::setAgentPosition(glm::vec3 pos)
{
	agent_.position = pos;
	agent_.updateTransform();
}

void MapWidget::setAgentColor(glm::vec3 color)
{
	agent_.color = color;
}

void MapWidget::setAgentCarry(bool value)
{
	if (value)
		agent_.texture = getTexture("guy_carry");
	else
		agent_.texture = getTexture("guy");
}

glm::vec3 MapWidget::getAgentPosition() const
{
	auto position = agent_.position;
	position.z = 0.1f;
	return position;
}

glm::vec3 MapWidget::getTilePosition(Pos2D tilePos) const
{
	return {
		(windowWidth_  - mapWidth_  * tileScale_) * .5f + tilePos.c * tileScale_,
		(windowHeight_ - mapHeight_ * tileScale_) * .5f + (mapHeight_ - 1 - tilePos.l) * tileScale_,
		0.0
	};
}

glm::vec3 MapWidget::getTileScale() const
{
	return {tileScale_, tileScale_, 1.f};
}

void MapWidget::openChest()
{
	tiles_.at(chestIndex_).texture = getTexture("chest_open");
}

void MapWidget::closeChest()
{
	tiles_.at(chestIndex_).texture = getTexture("chest");
}

void MapWidget::onRender()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	for (auto tile : tiles_)
	{
		renderer_.push(tile);
	}
	renderer_.push(agent_);
}
