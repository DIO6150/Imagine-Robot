#pragma once

#include <stdint.h>
#include <iostream>

#include <string>
#include <map>
#include <vector>

#include <Robot/Common/Request.hpp>
#include <Robot/Common/Tile.hpp>
#include <Robot/Errors/Error.hpp>
#include <Robot/Utils/Pos.hpp>

#include <vendor/nlohmann/json.hpp>

class Map
{
public:
	JSONParserStatus parseJSON(nlohmann::json const & object);

	Tile getTile(int32_t x, int32_t y) const;
	Tile getTile(Pos2D pos) const;

	bool isInside(Pos2D pos) const;
	
	std::string getName();
	uint32_t getWidth();
	uint32_t getHeight();
	Pos2D getRobotStart();
	Pos2D getPosStash();
	Pos2D getPosDictionary();
	std::vector<Resident> getResidentList();

	void mapInfos();

private:
	std::string const FORMAT_  = "robot-reconfort/carte";
	int         const VERSION_ = 1;

	std::string name_;
	uint32_t width_;
	uint32_t height_;
	std::map<char, TileProperty> charProperties_;
	std::vector<Tile> tiles_;
	Pos2D robotStart_;
	Pos2D posStash_;
	Pos2D posDictionary_;
	std::vector<Resident> residentList_;

	friend class MapParser;
};
