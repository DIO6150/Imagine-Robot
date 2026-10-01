#pragma once

#include <Robot/Errors/Error.hpp>

#include <Robot/Common/Tile.hpp>

class Playground;

enum class Orientation
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
	case Orientation::North: return Pos2D {0,  1};
	case Orientation::South: return Pos2D {0, -1};
	case Orientation::East:  return Pos2D { 1, 0};
	case Orientation::West:  return Pos2D {-1, 0};
	}

	return Pos2D {0, 0};
}

struct AgentTileView
{
	Tile north_;
	Tile east_;
	Tile west_;
	Tile south_;
};

struct AgentCommandListener
{
	virtual CommandResult<void> move(Orientation direction) = 0;

	virtual CommandResult<void> consult() = 0;
	virtual CommandResult<void> search(Orientation direction) = 0;

	virtual CommandResult<void> take(std::string name) = 0;
	virtual CommandResult<void> give() = 0;

	virtual CommandResult<AgentTileView> see() = 0;

	virtual void wait() = 0;
};