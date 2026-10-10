#pragma once

#include <functional>
#include <queue>
#include <optional>
#include <iostream>
#include <queue>
#include <cstdint>

#include <Robot/Agent/AgentControllerListener.hpp>
#include <Robot/Common/Request.hpp>
#include <Robot/Common/Item.hpp>
#include <Robot/Simulation/Map.hpp>

class Brain
{
public:
	Brain(AgentControllerListener * listener);

	void start(std::vector<Request> requestList);
	void tick();

private:
	void initalizeMentalMap(uint32_t height, uint32_t width);
	void updateMentalMap();

	int32_t bfs(std::vector<Pos2D> & chemin, Pos2D start, Pos2D destination);

private:
	std::vector<Tile> mentalArrangement_;
	std::vector<int32_t> mentalTileDistances_;
	std::vector<Request> requests_;
	
	std::string wantedItem_;

	Pos2D dictionary_;
	Pos2D itemStash_;
	Pos2D currentResident_;

private:
	AgentControllerListener * body_;
};