#pragma once

#include <stdint.h>

#include <string>

#include <Robot/Utils/Pos.hpp>

class TileProperty
{
public:
 	TileProperty & setSolid(bool value);
    TileProperty & setAgentStart(bool value);
    TileProperty & setPerson(bool value);
    TileProperty & setItemPickup(bool value);
    TileProperty & setRecord(bool value);

	bool isEmpty() const;
	bool isSolid() const;
	bool isAgentStart() const;
	bool isPerson() const;
	bool isItemPickup() const;
	bool isRecord() const;

private:
	enum : uint64_t
    {
        Solid        = 1ull << 0,
        AgentStart   = 1ull << 1,
        Person       = 1ull << 2,
        ItemPickup   = 1ull << 3,
        Record = 1ull << 4,
    };

	uint64_t flags_ = 0;
};

struct Tile: public TileProperty
{
	Tile(TileProperty const & property, Pos2D pos);

	Pos2D pos_;
};
