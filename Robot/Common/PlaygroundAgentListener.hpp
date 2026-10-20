#pragma once

#include <Robot/Utils/Pos.hpp>
#include <Robot/Common/Tile.hpp>
#include <Robot/Common/Emotion.hpp>

#include <vector>
#include <string>

struct PlaygroundAgentListener
{
	virtual bool canMove() = 0;
	virtual std::vector<Tile> see() = 0;
	virtual std::tuple<Emotion, Intensity> consult(std::string message) = 0;

	virtual std::string search(Pos2D posDrawer) = 0;
	virtual bool take(Pos2D posDrawer, std::string itemName) = 0;
};