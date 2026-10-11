#pragma once

#include <Robot/Errors/Error.hpp>

#include <nlohmann/json.hpp>

class ItemStash;

struct ClosetParser
{
	ClosetParser(ItemStash & stash, nlohmann::json const & object);
	JSONParserStatus result;
};