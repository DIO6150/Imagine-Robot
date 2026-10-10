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

Resident Request::getResident()
{
    return resident_;
}

std::string Request::getMessage()
{
    return message_;
}

void Request::setResident(Resident resident)
{
    resident_.id_ = resident.id_;
    resident_.name_ = resident.name_;
    resident_.pos_ = resident.pos_;
}