
#include <Robot/Simulation/Playground.hpp>

#include <vendor/nlohmann/json.hpp>

#include <iostream>
#include <fstream>

Playground::Playground()
{
	agent_ = std::make_unique<Agent>(this);
	view_  = std::make_unique<PlaygroundView>(this);
}

json dataParser(std::string const & path)
{
	std::ifstream Stream {path};
	json Data = json::parse(Stream);
	return Data;
}

int Playground::start(Directive const & instructions)
{

	auto status = script_.parseJSON(dataParser(instructions.script));

	if (status != JSONParserStatus::Ok)
		return 1;

	status = map_.parseJSON(dataParser(instructions.map));

	if (status != JSONParserStatus::Ok)
		return 1;

	if (script_.getMapName() != map_.getName())
		return 1;

	std::string closetPath = instructions.data + "/" + script_.getClosetName() + ".json";
	status = closet_.parseJSON(dataParser(closetPath));

	if (status != JSONParserStatus::Ok)
		return 1;

	std::string dictionaryPath = instructions.data + "/dictionnaire.json";
	status = dictionary_.parseJSON(dataParser(dictionaryPath));

	if (status != JSONParserStatus::Ok)
		return 1;

	RecapMap recap;

	recap.width_ = map_.getWidth();
	recap.height_ = map_.getHeight();
	recap.robotStart = map_.getRobotStart();
	recap.posStash = map_.getPosStash();
	recap.posDictionary = map_.getPosDictionary();
	recap.posResidentList = map_.getResidentList();

	agent_->start(script_.getRequests(), recap); // we suppose we have parsed the script and that we're passing it as arg to Agent::start
	return 0;
}

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

CommandResult<void> Playground::take(std::string name)
{
	return CommandStatus::Ok;
}

CommandResult<void> Playground::give()
{
	return CommandStatus::Ok;
}

CommandResult<AgentTileView> Playground::see()
{
	Orientation const dirs[] = {
		Orientation::North, Orientation::South, Orientation::West, Orientation::East
	};

	auto agentPos = agent_->getPosition();

	TileProperty solid;
	solid.setSolid(true);

	std::vector<Tile> result;
	for (auto dir : dirs)
	{
		auto pos = agentPos + orientationToPos(dir);
		if (!map_.isInside(pos)) // normally not possible but we never know ...
		{
			result.emplace_back(solid, pos);
			// TODO: push to trace (view)
		}

		auto tile = map_.getTile(pos);
		result.push_back(tile);
	}

	AgentTileView tileView;
	tileView.north_ = result.at(0);
	tileView.south_ = result.at(1);
	tileView.west_  = result.at(2);
	tileView.east_  = result.at(3);

	return tileView;
}

void Playground::wait()
{
	// litterally do nothing
}
