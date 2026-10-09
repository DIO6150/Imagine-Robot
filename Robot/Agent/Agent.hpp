#pragma once

#include <iostream>
#include <queue>
#include <cstdint>

#include <Robot/Common/AgentCommandListener.hpp>
#include <Robot/Common/Request.hpp>
#include <Robot/Simulation/Map.hpp>

struct RecapMap
{
	uint32_t width;
	uint32_t height;
	Pos2D robotStart;
	Pos2D posStash;
	Pos2D posDictionary;
	std::vector<Resident> posResidentList;
}

class Agent
{
public:
	Agent(AgentCommandListener * listener);

	void start(std::vector<Request> list_requests);
	void tick();

	Pos2D getPosition() const;
	void setPosition(Pos2D newPos);

	void initalizeMentalMap(RecapMap recap);

private:
	std::vector<int32_t> mentalMap_;
	Pos2D pos_;
	std::vector<Request> requests_;

private:
	AgentCommandListener * listener_;
};