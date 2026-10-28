#pragma once

#include <Robot/Common/Emotion.hpp>
#include <Robot/Utils/Pos.hpp>

#include <string>

struct Item
{
	std::string name;
	Emotion     emotion;
	Intensity   intensity;

	Pos2D pos;
};