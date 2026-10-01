#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include <Robot/Errors/Error.hpp>
#include <Robot/Simulation/Tile.hpp>

#include <vendor/nlohmann/json.hpp>

class Map
{
public:
	Error parseJson(nlohmann::json object);

private:


private:
	std::string name_;

	std::vector<Tile> tiles_;
	uint32_t width_;
	uint32_t height_;
};
