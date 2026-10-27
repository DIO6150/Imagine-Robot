
#pragma once

#include <Robot/Errors/Error.hpp>
#include <Robot/Simulation/Map.hpp>
#include <Robot/Utils/JSONParser.hpp>

class Map;

struct MapParser
{
	MapParser(Map & map, nlohmann::json const & object);
	JSONParserStatus result;
};