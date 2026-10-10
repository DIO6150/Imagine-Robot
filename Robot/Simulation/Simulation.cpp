
#include <Robot/Simulation/Simulation.hpp>

using json = nlohmann::json;

Simulation::Simulation()
{
	agent_ = std::make_unique<Agent>(this);
}

Script Simulation::getScript()
{
	return script_;
}

DirectiveTrace Simulation::executeDirective(Directive instructions)
{
	std::ifstream scriptStream {instructions.script};
	json scriptData = json::parse(scriptStream);
	auto status = script_.parseJSON(scriptData);
	
	if (status != JSONParserStatus::Ok)
		return 1;

	RecapMap recap = playground_.initalizePlayground(instructions);

	agent.initalizeAgent(recap);

	for(Request request : script_.getRequests())
	{
		agent_->tick();
	}

	/*
	while(1)
	{
		// if we have multiple SIMULATIONS MY NAMING IS WRONG AAAAAAAAAA
		// we would iterate trough and call tick() on each of them
		playground_.tick();
	
	}
	*/
	
	return DirectiveTrace {};
}

void Simulation::tick()
{
	agent_->tick();
}

CommandResult<void> Simulation::move(Orientation direction)
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

CommandResult<void> Simulation::consult()
{
	return CommandStatus::Ok;
}

CommandResult<void> Simulation::search(Orientation direction)
{
	return CommandStatus::Ok;
}

CommandResult<void> Simulation::take(std::string name)
{
	return CommandStatus::Ok;
}

CommandResult<void> Simulation::give()
{
	return CommandStatus::Ok;
}

CommandResult<AgentTileView> Simulation::see()
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

void Simulation::wait()
{
	// litterally do nothing
}

