#pragma once

#include <iostream>
#include <queue>
#include <cstdint>

#include <Robot/Common/AgentCommandListener.hpp>
#include <Robot/Common/Request.hpp>
#include <Robot/Simulation/Map.hpp>

class Agent
{
public:
	Agent(AgentCommandListener * listener);

	void start(std::vector<Request> list_requests);
	void tick();

	Pos2D getPosition() const;
	void setPosition(Pos2D newPos);

	void initalizeMentalMap(uint32_t height, uint32_t width);

private:
	std::vector<Tile> mentalArrangement_;
	std::vector<int32_t> mentalTileDistances_;
	Pos2D pos_;
	std::vector<Request> requests_;

private:
	AgentCommandListener * listener_;
};