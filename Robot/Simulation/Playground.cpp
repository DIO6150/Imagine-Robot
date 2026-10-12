
#include <iostream>
#include <fstream>

#include <Robot/Agent/AgentController.hpp>
#include <Robot/Simulation/Playground.hpp>

#include <vendor/nlohmann/json.hpp>


Playground::Playground()
{
	agent_ = std::make_unique<AgentController>(this);
	view_  = std::make_unique<PlaygroundView> (this);
}

void Playground::start(Directive const & instructions)
{
	JSONParserStatus status;
	
	status = parse(instructions.script, "scenario");
	if (status != JSONParserStatus::Ok) return;

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

	agent_->start(script_.getRequests(), map_.generateInfo(), closet_.getCasierDepart());
}

void Playground::tick()
{
	agent_->tick();
}

bool Playground::agentIsOver()
{
	return agent_->requestsOver();
}

bool Playground::agentCanMove()
{
	bool canMove = true;
	Pos2D pos = agent_->getPosition();
	if (!map_.isInside(pos))
	{
		// TODO: push to trace (view)
		canMove = false;
	}

	auto tile = map_.getTile(pos);
	if (tile.isSolid())
	{
		// TODO: push to trace (view)
		canMove = false;
	}
	return canMove;
}

std::vector<Tile> Playground::agentSee()
{
	std::vector<Orientation> const directions = { North, West, South, East };

	TileProperty wall;
	wall.setSolid(true);

	std::vector<Tile> result;
	Pos2D posAgent = agent_->getPosition();

	auto index = 0;
	for (auto & dir : directions)
	{
		auto iterator = result.begin() + index++;
		auto pos = posAgent + directionToPos(dir);

		if (!map_.isInside(pos))
		{
			result.emplace(iterator, Tile{wall, pos});
			continue;
		}

		result.emplace(iterator, map_.getTile(pos));
	}

	return result;
}

std::tuple<Emotion, Intensity> Playground::agentConsult(std::string message)
{
	std::tuple<Emotion, Intensity> result = dictionary_.identifEmoIntens(message);
	return result;
}

int Playground::agentSearch(Pos2D posDrawer)
{
	return ((closet_.getItem(posDrawer) == "vide") ? -1 : 1);
}

void Playground::agentTake(Pos2D posDrawer, std::string itemName)
{
	closet_.removeItem(posDrawer, itemName);
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

