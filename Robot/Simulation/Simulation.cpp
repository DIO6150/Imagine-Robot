
#include <Robot/Simulation/Simulation.hpp>

#include <vendor/nlohmann/json.hpp>

#include <iostream>

using json = nlohmann::json;

DirectiveTrace Simulation::executeDirective(Directive instructions)
{
	std::ifstream map {instructions.map};
	json mapData = json::parse(map); // TODO catch error here

	auto status = playground_.map_.parseJson(mapData);
	
	return DirectiveTrace {};
}
