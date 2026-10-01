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

	Pos2D & operator+=(Pos2D const & rhs)
	{
		x += rhs.x;
		y += rhs.y;

		return *this;
	}

	friend Pos2D operator+(Pos2D lhs, Pos2D const & rhs)
	{
		lhs += rhs;

		return lhs;
	}
};