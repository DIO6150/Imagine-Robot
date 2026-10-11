
#include <Robot/Simulation/Playground.hpp>

#include <vendor/nlohmann/json.hpp>

#include <fstream>

Playground::Playground()
{
	//agent_ = std::make_unique<Agent>(this);
	view_  = std::make_unique<PlaygroundView>(this);
}

Map Playground::getMap()
{
	return map_;
}

ItemStash Playground::getCloset()
{
	return closet_;
}

Dictionary Playground::getDictionary()
{
	return dictionary_;
}

JSONParserStatus Playground::dataParser(std::string const & path, std::string toFill)
{
	std::ifstream Stream {path};
	json Data = json::parse(Stream);
	JSONParserStatus status = JSONParserStatus::IncorrectArgument;
	if (toFill == "carte")
		status = map_.parseJSON(Data);
	else if (toFill == "armoire")
		status = closet_.parseJSON(Data);
	else if (toFill == "dictionnaire")
		status = dictionary_.parseJSON(Data);
	else
		std::cout << "incorrect element to fill" << std::endl;
	return status;
}

RecapMap Playground::initalizePlayground(Directive const & instructions)
{
	auto status = dataParser(instructions.map, "carte");

	if (status != JSONParserStatus::Ok)
		return 1;

	if (script_.getMapName() != map_.getName())
		return 1;

	std::string closetPath = instructions.data + "/" + script_.getClosetName() + ".json";
	status = dataParser(closetPath, "armoire");

	if (status != JSONParserStatus::Ok)
		return 1;

	std::string dictionaryPath = instructions.data + "/dictionnaire.json";
	status = dataParser(dictionaryPath, "dictionnaire");

	if (status != JSONParserStatus::Ok)
		return 1;

	RecapMap recap;

	recap.width_ = map_.getWidth();
	recap.height_ = map_.getHeight();
	recap.robotStart = map_.getRobotStart();
	recap.posStash = map_.getPosStash();
	recap.posDictionary = map_.getPosDictionary();
	recap.posResidentList = map_.getResidentList();

	//agent_->start(script_.getRequests(), recap); // we suppose we have parsed the script and that we're passing it as arg to Agent::start
	return recap;
}

/*
void Playground::tick()
{
	agent_->tick();
}

CommandResult<void> Playground::move(Orientation direction)
{
	Pos2D newPos = agent_->getPosition() + orientationToPos(direction);

	if (!map_.isInside(newPos))
	{
		// TODO: push to trace (view)
		return CommandStatus::AgentMovementObstructed;
	}

	auto tile = map_.getTile(newPos);
	if (tile.isSolid())
	{
		// TODO: push to trace (view)
		return CommandStatus::AgentMovementObstructed;
	}

	//agent_->move(newPos);

	// TODO: push to trace (view)

	return CommandStatus::Ok;
}

CommandResult<void> Playground::consult()
{
	return CommandStatus::Ok;
}

CommandResult<void> Playground::search(Orientation direction)
{
	return CommandStatus::Ok;
}

CommandResult<Item> Playground::take(std::string name)
{
	return CommandStatus::Ok;
}

CommandResult<void> Playground::give(Item item)
{
	return CommandStatus::Ok;
}

CommandResult<Tile> Playground::see(Pos2D pos)
{
	if (!map_.isInside(pos))
		return CommandStatus::OutOfBounds;

	return map_.getTile(pos);
}

void Playground::onAgentFail(AgentFailureType type, std::string message)
{
	std::cout << "AGENT FAILURE" << failureToString(type) << " " << message; // TODO: print it in the trace too
}

void Playground::wait()
{
	// litterally do nothing
}
*/
