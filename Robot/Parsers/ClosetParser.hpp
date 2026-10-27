#pragma once

#include <Robot/Simulation/ItemStash.hpp>
#include <Robot/Utils/JSONParser.hpp>
#include <Robot/Errors/Error.hpp>

class ItemStash;

struct ClosetParser
{
	ClosetParser(ItemStash & stash, nlohmann::json const & object);
	JSONParserStatus result;
};