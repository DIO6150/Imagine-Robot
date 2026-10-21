#pragma once
#include <Robot/Utils/Pos.hpp>

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
	case North: return Pos2D { -1,  0 };
	case South: return Pos2D { 1, 0 };
	case West:  return Pos2D { 0, -1 };
	case East:  return Pos2D { 0, 1 };
	}

	return Pos2D {0, 0};
}

inline Orientation findDirection(Pos2D a, Pos2D b)
{
	auto dir = a - b;

	if (dir.l < 0) return North;
	else if (dir.l > 0) return South;

	if (dir.c > 0) return East;
	else if (dir.c < 0) return West;

	return North;
}