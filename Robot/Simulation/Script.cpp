
#include <Robot/Simulation/Script.hpp>

JSONParserStatus Script::parseJSON(json const & object)
{
	return parser_.fillFields(object);
}

std::vector<Request> Script::getRequests()
{
    return requests_;
}

std::string Script::getMapName()
{
    return mapName_;
}

std::string Script::getClosetName()
{
    return closetName_;
}