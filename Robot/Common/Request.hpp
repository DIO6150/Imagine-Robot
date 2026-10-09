#pragma once

#include <string>
#include <cstdint>

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
	std::string getResidentId();
	std::string getMessage();

private:
	Resident resident;
	uint32_t id_;
	std::string message_;
};