
#include <Robot/Render/PlaygroundView.hpp>

#include <Robot/Utils/Easing.hpp>

#include <glm/gtc/matrix_transform.hpp>


PlaygroundView::PlaygroundView(PlaygroundViewListener * listener)
	: listener_ {listener}
{
	
}

void PlaygroundView::init(Map const & map, AgentController const * agent, GLFWwindow * window)
{
	window_ = window;

	resources_.init();
	mapRenderer_.init(&resources_);
	mapRenderer_.setMap(map.rawTileMap(), map.getWidth(), map.getHeight());

	itemRenderer_.init(&resources_);

	agent_ = agent;
	map_ = &map;
}

void PlaygroundView::update(double deltaTime)
{
	deltaTime_ = deltaTime;
}

void PlaygroundView::draw()
{
	if (equipedItem_)
	{
		itemRenderer_.render(equipedItem_.value());
	}

	mapRenderer_.draw();
	itemRenderer_.draw();
}

glm::vec2 PlaygroundView::convertPos(Pos2D tilePos)
{
	glm::vec2 pos;

	pos.x = tilePos.c;
	pos.y = (map_->getHeight() - 1) - tilePos.l;

	return pos;
}

Animation PlaygroundView::moveAgent(Pos2D oldPos, Pos2D newPos)
{
	glm::vec2 start = mapRenderer_.from(oldPos);
	glm::vec2 target = mapRenderer_.from(newPos);

	return [this, start, target, progress = 0.0f]() mutable
	{
		const float duration = .5f / simulationSpeed_;

		progress += static_cast<float>(deltaTime_) / duration;

		float t = easeInOutCirc(progress);

		glm::vec2 pos = glm::mix(start, target, t);
		mapRenderer_.setAgentPosition(pos);

		if (progress >= 1.0f)
		{
			mapRenderer_.setAgentPosition(target);
			return Stop;
		}

		return Yield;
	};
}

Animation PlaygroundView::equipItem(Item item)
{
	return [this, item, progress = 0.0f]() mutable
	{
		const float duration = .1f / simulationSpeed_;

		progress += static_cast<float>(deltaTime_) / duration;

		if (progress >= 1.0f)
		{	
			equipedItem_ = itemRenderer_.createItem(item.emotion, item.intensity);
			auto & transform = equipedItem_->transform;
			
			transform = glm::translate(transform, {-1.f, 1.f - .15f, .0f});
			transform = glm::scale(transform, {.15f, .15f, 1.f});
			return Stop;
		}

		return Yield;
	};
}

Animation PlaygroundView::unequipItem()
{
	return [this, progress = 0.0f]() mutable
	{
		const float duration = .2f / simulationSpeed_;

		progress += static_cast<float>(deltaTime_) / duration;

		if (progress >= 1.0f)
		{
			equipedItem_ = std::nullopt;
			return Stop;
		}

		return Yield;
	};
}