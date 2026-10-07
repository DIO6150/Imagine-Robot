#pragma once

#include <iostream>

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

	Request getCurrentRequest();
	void removeRemoveRequest();

	void initalizeMentalMap(int height, int width);

private:
	std::vector<std::tuple<Tile, int>> MentalMap;
	Pos2D pos_;
	std::vector<Request> requests_;

private:
	AgentCommandListener * listener_;
};