#pragma once

#include <stdint.h>
#include <iostream>

#include <string>
#include <map>
#include <vector>

#include <Robot/Common/MapInfo.hpp>
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
	
	std::string getName() const;
	uint32_t getWidth() const;
	uint32_t getHeight() const;
	Pos2D getRobotStartPos() const;
	Pos2D getStashPos() const;
	Pos2D getDictionaryPos() const;
	Resident atResident(std::string idRes) const;

	MapInfo generateInfo() const;

	void printInfo() const;

private:
	std::string const FORMAT_  = "robot-reconfort/carte";
	int         const VERSION_ = 1;

	std::string name_;
	uint32_t width_;
	uint32_t height_;
	std::map<char, TileProperty> charProperties_;
	std::vector<Tile> tiles_;
	Pos2D robotStartPos_;
	Pos2D stashPos_;
	Pos2D dictionaryPos_;
	std::map<std::string, Resident> residents_;

	friend class MapParser;
};

