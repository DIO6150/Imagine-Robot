#include <Robot/Common/Request.hpp>

uint32_t Request::getId()
{
    return id_;
}

std::string Request::getResidentName()
{
    return residentName_;
}

std::string Request::getMessage()
{
    return message_;
}
	
void Request::setId(uint32_t id)
{
    id_ = id;
}

void Request::setResidentName(std::string residentName)
{
    residentName_ = residentName;
}

void Request::setMessage(std::string message)
{
    message_ = message;
}