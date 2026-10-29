#pragma once

#include <Robot/Utils/Pos.hpp>

#include <Robot/Common/Tile.hpp>
#include <Robot/Common/Emotion.hpp>
#include <Robot/Common/MapInfo.hpp>
#include <Robot/Common/Item.hpp>

#include <vector>
#include <string>

struct PlaygroundAgentListener
{
	virtual bool canMove() = 0;
	virtual std::vector<Tile> see() = 0;
	virtual std::tuple<Emotion, Intensity> consult(std::string message) = 0;

	virtual std::optional<Item> search(Pos2D posDrawer) = 0;
	virtual std::optional<Item> take(Pos2D posDrawer) = 0;


	// Events
	virtual void onMapInit(std::vector<Tile> map, MapInfo info) = 0;

	virtual void onAgentMove(Pos2D oldPos, Pos2D newPos) = 0;

	virtual void onAgentTakeItem(Item item) = 0;
	virtual void onAgentGiveItem(Item item, Pos2D resident) = 0;

	virtual void onAgentStashOpen () = 0;
	virtual void onAgentStashClose() = 0;

	virtual void onAgentConsult() = 0;

	virtual void onAgentSelect(Pos2D old, Pos2D cursor) = 0;
};
