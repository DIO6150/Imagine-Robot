#include <Robot/Common/Request.hpp>

Request::Request(uint32_t id, Resident resident, std::string message)
{
    id_ = id;
    resident_.id_ = resident.id_;
    resident_.name_ = resident.name_;
    resident_.pos_ = resident.pos_;
    message_ = message;
}
    
uint32_t Request::getId()
{
    return id_;
}

std::string Request::getResidentId()
{
    return resident.id_;
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
    resident.id_ = residentId;
}

void Request::setMessage(std::string message)
{
    message_ = message;
}