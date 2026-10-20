#pragma once

#include <functional>
#include <queue>
#include <map>
#include <optional>
#include <iostream>
#include <cstdint>

#include <Robot/Agent/AgentController.hpp>
#include <Robot/Common/MapInfo.hpp>
#include <Robot/Common/Request.hpp>
#include <Robot/Common/Item.hpp>
#include <Robot/Simulation/Map.hpp>

enum class Objective : int32_t
{
	ReachDictionary,
	InterpretRequest,
	ReachCloset,
	FindItem,
	TakeItem,
	ReachResident,
	GiveItem,
	NoObjective,
};


class Brain
{
public:
	Brain(AgentControllerListener * listener);

	void start(std::vector<Request> requestList, MapInfo info, Pos2D casierDepart);
	void tick();

	std::string getCurrentMessage();
	bool noRequestsLeft();

private:
	void initalizeMentalMap();
	void simulationProgress();

	std::queue<Pos2D> shortestPath(Pos2D start, Pos2D destination);
	std::queue<Pos2D> closetPath();

private:
	MapInfo mapInfo_;
	std::vector<Request>      requestList_;
	int                       currentReq_;
	std::vector<Tile>         mentalMap_;
	std::vector<bool>         isVisited_;
	std::queue<Pos2D>         currentPath_;
	bool                      destinationReached_;
	Emotion                   currentSearchedEmotion_;
	Intensity                 currentSearchedIntensity_;
	std::queue<Pos2D>         drawerCheckQueue_;
	std::vector<std::string>  drawersContainsItem_ = std::vector<std::string>(24, "not seen");


	Objective currentObjective_ = Objective::ReachDictionary;

	bool carryItem_ = false;
	std::string itemName_;

	Pos2D tileTarget_;
	Pos2D pos_;
	Pos2D casier_;


private:
	AgentControllerListener * body_;
};
