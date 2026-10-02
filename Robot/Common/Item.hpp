#pragma once

#include <Robot/Common/Emotion.hpp>
#include <Robot/Utils/Pos.hpp>

#include <string>

class Item
{
	std::string name_;
	Emotion emotion_;
	Intensity Intensity_;

	Pos2D pos_;
};