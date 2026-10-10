
#include <iostream>
#include <fstream>

#include <Robot/Agent/AgentController.hpp>
#include <Robot/Simulation/Playground.hpp>

#include <vendor/nlohmann/json.hpp>


Playground::Playground()
{
	agent_ = std::make_unique<AgentController>(map_);
	view_  = std::make_unique<PlaygroundView> (this);
}

json dataParser(std::string const & path)
{
	std::ifstream Stream {path};
	json Data = json::parse(Stream);
	return Data;
}

void Playground::start(Directive const & instructions)
{
	auto status = map_.parseJSON(dataParser(instructions.map));

	if (status != JSONParserStatus::Ok)
		return;

	status = dictionary_.parseJSON(dataParser(instructions.dictionary));

	if (status != JSONParserStatus::Ok)
		return;

	status = closet_.parseJSON(dataParser(instructions.closet));

	if (status != JSONParserStatus::Ok)
		return;

	status = script_.parseJSON(dataParser(instructions.script));

	if (status != JSONParserStatus::Ok)
		return;

	agent_->start(script_.getRequests()); // we suppose we have parsed the script and that we're passing it as arg to Agent::start
}

void Playground::tick()
{
	agent_->tick();
}

