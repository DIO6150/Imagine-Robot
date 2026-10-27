#pragma once

#include <stdint.h>

struct Pos2D
{
	int32_t l = 0;
	int32_t c = 0;

	bool operator==(Pos2D const & rhs) const
	{
		return l == rhs.l && c == rhs.c;
	}

	bool operator<(Pos2D const & rhs) const
	{
		if (l != rhs.l)
        	return l < rhs.l;
		return c < rhs.c;
	}

	Pos2D & operator+=(Pos2D const & rhs)
	{
		l += rhs.l;
		c += rhs.c;

		return *this;
	}

	Pos2D & operator-=(Pos2D const & rhs)
	{
		l -= rhs.l;
		c -= rhs.c;

		return *this;
	}

	friend Pos2D operator+(Pos2D lhs, Pos2D const & rhs)
	{
		lhs += rhs;

		return lhs;
	}

	friend Pos2D operator-(Pos2D lhs, Pos2D const & rhs)
	{
		lhs -= rhs;

		return lhs;
	}
};