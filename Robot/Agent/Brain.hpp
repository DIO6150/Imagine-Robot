#pragma once

#include <functional>
#include <queue>
#include <optional>
#include <iostream>
#include <queue>
#include <cstdint>

#include <Robot/Agent/AgentControllerListener.hpp>
#include <Robot/Common/MapInfo.hpp>
#include <Robot/Common/Request.hpp>
#include <Robot/Common/Item.hpp>
#include <Robot/Simulation/Map.hpp>

enum class Objective : int32_t
{
	InterpretRequest,
	FindDictionary,
	FindItem,
	TakeItem,
	FindPerson,
	GiveItem,
};


class Brain
{
public:
	Brain(AgentControllerListener * listener);

	void start(MapInfo info, std::vector<Request> requestList);
	void tick();

private:
	void initalizeMentalMap();

	std::vector<Pos2D> shortestPath(Pos2D start, Pos2D destination);

	bool move(Orientation direction);

private:
	//Request currentRequest_;
	MapInfo mapInfo_;
	std::vector<Tile>  mentalMap_;
	std::vector<Pos2D> currentPath_;


	Objective currentObjective_ = Objective::FindDictionary;

	bool carryItem_ = false;
	std::string itemName_;

	Pos2D tileTarget_;
	

	Pos2D pos_;


private:
	AgentControllerListener * body_;
};