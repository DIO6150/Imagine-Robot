#include <Robot/Common/Request.hpp>

Request::Request(uint32_t id, std::string idResident, std::string message)
{
    id_ = id;
    idResident_ = idResident;
    message_ = message;
}
    
uint32_t Request::getId()
{
    return id_;
}

std::string Request::getIdResident()
{
    return idResident_;
}

std::string Request::getMessage()
{
    return message_;
}
