#pragma once

#include <stdint.h>

struct Pos2D
{
	int32_t x = 0;
	int32_t y = 0;

	bool operator==(Pos2D const & rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
};