#pragma once

#include <Robot/Errors/Error.hpp>

#include <Robot/Common/Tile.hpp>
#include <Robot/Common/Item.hpp>

class Playground;

enum Orientation
{
	North,
	East,
	West,
	South
};

inline Pos2D orientationToPos(Orientation direction)
{
	switch (direction)
	{
	case North: return Pos2D { 0,  1 };
	case South: return Pos2D { 0, -1 };
	case West:  return Pos2D { -1, 0 };
	case East:  return Pos2D {  1, 0 };
	}

	return Pos2D {0, 0};
}

struct AgentControllerListener
{
	virtual CommandResult<void> move(Orientation direction) = 0;

	virtual CommandResult<void> consult() = 0;
	virtual CommandResult<void> search(Orientation direction) = 0;

	virtual CommandResult<Item> take(std::string name) = 0;
	virtual CommandResult<void> give() = 0;

	virtual CommandResult<std::vector<Tile>> see() = 0;

	virtual void onAgentFail(AgentFailureType type, std::string message) = 0;
};