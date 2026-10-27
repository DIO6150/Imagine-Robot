#pragma once

#include <Robot/Common/PlaygroundViewListener.hpp>

#include <Robot/Utils/Animation.hpp>

#include <Robot/Render/MapRenderer.hpp>

#include <Robot/Simulation/Map.hpp>
#include <Robot/Agent/AgentController.hpp>

class PlaygroundView
{
public:
	PlaygroundView(PlaygroundViewListener * listener);

	void init(Map const & map, AgentController const * agent);
	void update(double deltaTime);
	void draw();

	Animation moveAgent(Pos2D newPos);

private:
	glm::vec2 convertPos(Pos2D tilePos);

private:
	double deltaTime_;

	AgentController const * agent_;
	Map const * map_;

	MapRenderer physicalMap_;

	bool itemStashFocus_ = false;

private:
	PlaygroundViewListener * listener_;
};