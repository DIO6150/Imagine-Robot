#include <Robot/Common/Request.hpp>

uint32_t Request::getId()
{
    return id_;
}

std::string Request::getResidentId()
{
    return residentId_;
}

std::string Request::getMessage()
{
    return message_;
}
	
void Request::setId(uint32_t id)
{
    id_ = id;
}

void Request::setResidentId(std::string residentId)
{
    residentId_ = residentId;
}

void Request::setMessage(std::string message)
{
    message_ = message;
}