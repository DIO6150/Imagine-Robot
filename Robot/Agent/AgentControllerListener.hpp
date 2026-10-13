#pragma once

#include <vector>
#include <tuple>

#include <Robot/Errors/Error.hpp>

#include <Robot/Common/Tile.hpp>
#include <Robot/Common/Item.hpp>

enum Orientation
{
	North,
	East,
	West,
	South
};

inline Pos2D directionToPos(Orientation direction)
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

inline Orientation findDirection(Pos2D a, Pos2D b)
{
	auto dir = a - b;

	if (dir.y > 0) return North;
	else if (dir.y < 0) return South;

	if (dir.x > 0) return East;
	else if (dir.x < 0) return West;

	return North;
}

struct AgentControllerListener
{
	virtual CommandResult<void> move(Orientation direction) = 0;

	virtual CommandResult<std::tuple<Emotion, Intensity>> consult() = 0;
	virtual CommandResult<int> search(Orientation direction) = 0;

	virtual CommandResult<void> take(std::string name) = 0;
	virtual CommandResult<void> give(std::string name) = 0;

	virtual CommandResult<std::vector<Tile>> see() = 0;

	virtual void onAgentFail(AgentFailureType type, std::string message) = 0;
};
