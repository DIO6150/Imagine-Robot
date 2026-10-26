#pragma once

#include <functional>
#include <queue>
#include <map>
#include <optional>
#include <iostream>
#include <cstdint>

#include <Robot/Agent/AgentController.hpp>
#include <Robot/Common/Item.hpp>
#include <Robot/Common/MapInfo.hpp>
#include <Robot/Common/Request.hpp>
#include <Robot/Common/TraceElement.hpp>
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

	std::vector<Pas> getSteps();
	std::vector<Livraison> getDeliveries();

	std::string getCurrentMessage();
	bool noRequestsLeft();

	static std::queue<Pos2D> shortestPath(Pos2D start, Pos2D destination, std::vector<Tile> givenMap, uint32_t width);

private:
	void initalizeMentalMap();
	void simulationProgress();

	void addStep(Orientation dir);
	void addDelivery(std::string motif);
	void nextRequest(std::string motif);

	std::queue<Pos2D> closetPath();

private:
	std::vector<Pas>		  	 steps_;
	std::vector<Livraison>   	 deliveries_;

	uint32_t                  	 nSteps_;
	uint32_t                  	 nStepsRequest_;
	MapInfo					 	 mapInfo_;
	std::vector<Request>      	 requestList_;
	int                       	 currentReq_;
	std::vector<Tile>    		 mentalMap_;
	std::map<Orientation, Tile>  currentPerception_;
	std::queue<Pos2D>            currentPath_;
	bool                 	     destinationReached_;
	bool						 deliverySuccess_;
	Emotion               	     currentSearchedEmotion_;
	Intensity                	 currentSearchedIntensity_;
	std::queue<Pos2D>         	 drawerCheckQueue_;
	std::vector<std::string>  	 drawersContainsItem_ = std::vector<std::string>(24, "not seen");


	Objective currentObjective_ = Objective::ReachDictionary;

	bool carryItem_ = false;
	std::string itemName_;

	Pos2D tileTarget_;
	Pos2D pos_;
	Pos2D casier_;


private:
	AgentControllerListener * body_;
};
