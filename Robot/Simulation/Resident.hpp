#pragma once

#include <stdint.h>

#include <string>

#include <Robot/Utils/Pos.hpp>

struct Resident
{
	std::string id_;
	std::string name_;
	Pos2D pos_;
};