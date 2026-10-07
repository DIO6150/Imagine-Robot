#pragma once

#include <string>
#include <cstdint>

class Request
{
public:
	uint32_t getId();
	std::string getResidentName();
	std::string getMessage();
	
	void setId(uint32_t id);
	void setResidentName(std::string residentName);
	void setMessage(std::string message);

private:
	uint32_t id_;
	std::string residentName_;
	std::string message_;
};