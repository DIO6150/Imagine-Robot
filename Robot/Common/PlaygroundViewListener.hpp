#pragma once

#include <Robot/Common/MapInfo.hpp>
#include <Robot/Common/Tile.hpp>
#include <Robot/Common/Item.hpp>

#include <Robot/Utils/Pos.hpp>

struct PlaygroundViewListener
{
	virtual void onMapInit(std::vector<Tile> map, MapInfo info) = 0;

	virtual void onAgentMove(Pos2D newPos) = 0;

	virtual void onAgentTakeItem(Item item) = 0;
	virtual void onAgentGiveItem(Item item) = 0;

	virtual void onAgentStashOpen () = 0;
	virtual void onAgentStashClose() = 0;

	virtual void onAgentConsult() = 0;

	virtual void onAgentSelect(Pos2D cursor) = 0;
};