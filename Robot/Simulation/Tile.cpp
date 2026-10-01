
#include <Robot/Simulation/Tile.hpp>

// note to self (table 01/10/26)
// not so sure about the flag |= and &=

TileProperty & TileProperty::setSolid(bool value)
{
    if (value)
        flags_ |= Solid;
    else
        flags_ &= ~Solid;

	return *this;
}

TileProperty & TileProperty::setAgentStart(bool value)
{
    if (value)
        flags_ |= AgentStart;
    else
        flags_ &= ~AgentStart;

	return *this;
}

TileProperty & TileProperty::setPerson(bool value)
{
    if (value)
        flags_ |= Person;
    else
        flags_ &= ~Person;

	return *this;
}

TileProperty & TileProperty::setItemPickup(bool value)
{
    if (value)
        flags_ |= ItemPickup;
    else
        flags_ &= ~ItemPickup;

	return *this;
}

TileProperty & TileProperty::setRecord(bool value)
{
    if (value)
        flags_ |= Record;
    else
        flags_ &= ~Record;

	return *this;
}

bool TileProperty::isSolid() const
{
    return flags_ & Solid;
}

bool TileProperty::isAgentStart() const
{
    return flags_ & AgentStart;
}

bool TileProperty::isPerson() const
{
    return flags_ & Person;
}

bool TileProperty::isItemPickup() const
{
    return flags_ & ItemPickup;
}

bool TileProperty::isRecord() const
{
    return flags_ & Record;
}

bool TileProperty::isEmpty() const
{
    return flags_ & 0;
}

Tile::Tile(TileProperty const & property, Pos2D pos)
	: TileProperty{property}
	, pos_{pos}
{

}