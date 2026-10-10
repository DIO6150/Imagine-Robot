
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

void Playground::start(Directive const & instructions)
{
	JSONParserStatus status;
	
	parse(instructions.script, "scenario");

	// TODO : DEBATE do we use the name in the script file or the name in the instruction set ?
	std::string mapPath        = instructions.map;
	std::string closetPath     = instructions.data + "/" + script_.getClosetName() + ".json";
	std::string dictionaryPath = instructions.data + "/dictionnaire.json";

	status = parse(mapPath, "carte");
	if (status != JSONParserStatus::Ok) return;
	if (script_.getMapName() != map_.getName()) return;

	status = parse(closetPath, "armoire");
	if (status != JSONParserStatus::Ok) return;

	status = parse(dictionaryPath, "dictionnaire");
	if (status != JSONParserStatus::Ok)	return;

	agent_->start(script_.getRequests());
}

void Playground::tick()
{
	agent_->tick();
}

JSONParserStatus Playground::parse(std::string const & path, std::string toFill)
{
	std::ifstream stream {path};
	json data   = json::parse(stream);
	auto status = JSONParserStatus::IncorrectArgument;

	if      (toFill == "carte")        status = map_       .parseJSON(data);
	else if (toFill == "scenario")     status = script_    .parseJSON(data);
	else if (toFill == "armoire")      status = closet_    .parseJSON(data);
	else if (toFill == "dictionnaire") status = dictionary_.parseJSON(data);
	else std::cout << "incorrect element to fill" << std::endl;

	return status;
}
