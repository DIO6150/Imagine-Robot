#pragma once

#include <string>
#include <cstdint>

class Request
{
public:
	uint32_t getId();
	std::string getResidentId();
	std::string getMessage();
	
	void setId(uint32_t id);
	void setResidentId(std::string residentId);
	void setMessage(std::string message);

private:
	uint32_t id_;
	std::string residentId_;
	std::string message_;
};