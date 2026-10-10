#pragma once

#include <algorithm>
#include <iostream>
#include <queue>
#include <cstdint>
#include <map>

#include <Robot/Common/AgentCommandListener.hpp>
#include <Robot/Common/Request.hpp>
#include <Robot/Common/Tile.hpp>

struct RecapMap
{
	uint32_t width_;
	uint32_t height_;
	Pos2D robotStart;
	Pos2D posStash;
	Pos2D posDictionary;
	std::vector<Resident> posResidentList;
};

class Agent
{
public:
	Agent(AgentCommandListener * listener);

	void start(std::vector<Request> list_requests, RecapMap recap);
	void tick();

	Pos2D getPosition() const;
	void setPosition(Pos2D newPos);

	void MessageIdentifyLever(bool state);
	void ItemRetrivalLever(bool state);
	void ItemDeliveryLever(bool state);

	void initalizeMentalMap();

	std::vector<Pos2D> shortestPath(Pos2D start, Pos2D destination);

	//CommandStatus<void> move(Orientation direction);

private:
	std::vector<Tile> mentalMap_;
	RecapMap recapMap_;
	Pos2D pos_;
	std::vector<Request> requests_;

	bool MessageIdentifyActive_ = false;
	bool ItemRetrivalActive_ = false;
	bool ItemDeliveryActive_ = false;

	void recapMapSet(RecapMap recap);
	Tile atMap(std::vector<Tile> & map, int32_t y, int32_t x);
	bool isOutWalls(int32_t x, int32_t y);


private:
	AgentCommandListener * listener_;
};