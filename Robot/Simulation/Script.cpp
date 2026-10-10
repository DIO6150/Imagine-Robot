
#include <Robot/Simulation/Script.hpp>
#include <Robot/Parsers/ScriptParser.hpp>


JSONParserStatus Script::parseJSON(json const & object)
{
	return ScriptParser {*this, object}.result;
}

std::vector<Request> Script::getRequests()
{
    return requests_;
}