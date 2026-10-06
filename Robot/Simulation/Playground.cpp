
#include <Robot/Simulation/Playground.hpp>

#include <vendor/nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

Playground::Playground()
{
	agent_ = std::make_unique<Agent>(this);
	view_  = std::make_unique<PlaygroundView>(this);
}

void Playground::start(Directive const & instructions)
{
	std::ifstream mapStream {instructions.map};
	json mapData = json::parse(mapStream); // TODO catch error here

	auto status = map_.parseJson(mapData);

	if (status != JSONParserStatus::Ok)
		return;

	agent_->start({}); // we suppose we have parsed the script and that we're passing it as arg to Agent::start
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

	agent_->setPosition(newPos);

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
