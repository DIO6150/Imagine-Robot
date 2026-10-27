
#include <Robot/Render/PlaygroundView.hpp>

#include <glad/glad.h>

#include <Robot/Utils/Easing.hpp>


PlaygroundView::PlaygroundView(PlaygroundViewListener * listener)
	: listener_ {listener}
{
	
}

void PlaygroundView::init(Map const & map, AgentController const * agent)
{
	physicalMap_.init();
	physicalMap_.setMap(map.rawTileMap(), map.getWidth(), map.getHeight());

	agent_ = agent;
	map_ = &map;
}

void PlaygroundView::update(double deltaTime)
{
	deltaTime_ = deltaTime;
}

void PlaygroundView::draw()
{
	/*
		Draw Map here
	*/
	physicalMap_.draw();

	/*
		Draw Agent here
	*/

	/*
		Draw Item Stash here
	*/
}

glm::vec2 PlaygroundView::convertPos(Pos2D tilePos)
{
	glm::vec2 pos;

	pos.x = tilePos.c;
	pos.y = (map_->getHeight() - 1) - tilePos.l;

	return pos;
}

Animation PlaygroundView::moveAgent(Pos2D newPos)
{
	Pos2D start  = agent_->getPosition();
	Pos2D target = newPos; 

	return [this, start, target, progress = 0.0f]() mutable
	{
		constexpr float duration = 0.05f;

		progress += static_cast<float>(deltaTime_) / duration;

		if (progress >= 1.0f)
		{
			physicalMap_.setAgentPosition(target);
			return Stop;
		}

		return Yield;
	};
}
