#pragma once

#include <string>
#include <cstdint>
#include <Robot/Utils/Pos.hpp>

struct Resident
{
	std::string name_;
	Pos2D pos_;
};

class Request
{
public:
	Request() = default;
	Request(uint32_t id, std::string idResident, std::string message);
	uint32_t getId();
	std::string getIdResident();
	std::string getMessage();

private:
	uint32_t id_;
	std::string idResident_;
	std::string message_;
};
