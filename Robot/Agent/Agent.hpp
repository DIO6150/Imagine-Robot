#pragma once

#include <Robot/Common/AgentCommandListener.hpp>
#include <Robot/Simulation/Map.hpp>

struct Request
{
	uint32_t id_;
	std::string resident_;
	std::string message_;
};

class Agent
{
public:
	Agent(AgentCommandListener * listener);

	void start(std::initializer_list<Request> list_requests);
	void tick();

	Pos2D getPosition() const;
	void setPosition(Pos2D newPos);

	Request getCurrentRequest() const;
	void removeRemoveRequest();

	std::vector<std::tuple<Tile, int>> initalizeMentalMap(int height, int width);

private:
	std::vector<std::tuple<Tile, int>> MentalMap;
	Pos2D pos_;
	std::vector<Request> requests_;

private:
	AgentCommandListener * listener_;
};