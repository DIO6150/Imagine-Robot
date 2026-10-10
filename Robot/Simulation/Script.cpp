
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

void Script::scriptInfos()
{
    std::cout << "Nom script : " << name_ << std::endl;
	std::cout << "Nom carte : " << mapName_ << std::endl;
	std::cout << "Nom armoire : " << closetName_ << std::endl;
	for (Request req : requests_)
		std::cout << "Requete " << req.getId() << std::endl;
}