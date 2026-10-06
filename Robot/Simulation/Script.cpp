
#include <Robot/Simulation/Script.hpp>

JSONParserStatus Script::parseJson(json const & object)
{
	return parser_.fillFields(object);
}


std::vector<Request> Script::getRequests()
{
    return requests_;
}