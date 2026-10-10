#pragma once

#include <string>
#include <cstdint>
#include <Robot/Utils/Pos.hpp>

struct Resident
{
	std::string id_;
	std::string name_;
	Pos2D pos_;
};

class Request
{
public:
	Request(uint32_t id, Resident resident, std::string message);
	uint32_t getId();
	Resident getResident();
	std::string getMessage();

	void setResident(Resident resident);

private:
	Resident resident_;
	uint32_t id_;
	std::string message_;
};