
#pragma once

#include <Robot/Errors/Error.hpp>

#include <nlohmann/json.hpp>

class Map;

struct MapParser
{
	MapParser(Map & map, nlohmann::json const & object);
	JSONParserStatus result;
};