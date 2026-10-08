
#include <Robot/Simulation/Playground.hpp>

#include <vendor/nlohmann/json.hpp>

#include <iostream>
#include <fstream>

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
