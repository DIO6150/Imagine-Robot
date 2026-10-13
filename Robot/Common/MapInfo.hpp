#pragma once

#include <Robot/Common/Request.hpp>

#include <Robot/Utils/Pos.hpp>

#include <map>
#include <vector>

struct MapInfo
{
	uint32_t width;
	uint32_t height;
	Pos2D robotStartPos;
	Pos2D stashPos;
	Pos2D dictionaryPos;
	std::map<std::string, Resident> residents_; // is it really a residentList ?
};
