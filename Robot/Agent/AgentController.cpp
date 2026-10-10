
#include <Robot/Agent/AgentController.hpp>
#include <Robot/Agent/Brain.hpp>

#include <Robot/Simulation/Map.hpp>


AgentController::AgentController(Map const & map)
	: map_ {map}
{
	brain_ = std::make_unique<Brain>(this);
}

void AgentController::start(std::vector<Request> requestList)
{
	brain_->start(requestList);
}

void AgentController::tick()
{
	brain_->tick();
}

CommandResult<void> AgentController::move(Orientation direction)
{
	Pos2D pos = pos_ + orientationToPos(direction);

	if (!map_.isInside(pos))
	{
		// TODO: push to trace (view)
		return CommandStatus::AgentMovementObstructed;
	}

	auto tile = map_.getTile(pos);
	if (tile.isSolid())
	{
		// TODO: push to trace (view)
		return CommandStatus::AgentMovementObstructed;
	}

	pos_ = pos;

	return CommandStatus::Ok;
}

CommandResult<void> AgentController::consult()
{
	return CommandStatus::Ok;
}

CommandResult<void> AgentController::search(Orientation direction)
{
	return CommandStatus::Ok;
}

CommandResult<Item> AgentController::take(std::string name)
{
	return CommandStatus::Ok;
}

CommandResult<void> AgentController::give()
{
	return CommandStatus::Ok;
}

CommandResult<std::vector<Tile>> AgentController::see()
{
	std::vector<Orientation> const directions = { North, West, South, East };

	TileProperty wall;
	wall.setSolid(true);

	std::vector<Tile> result {4};

	auto index = 0;
	for (auto & dir : directions)
	{
		auto iterator = result.begin() + index++;
		auto pos = pos_ + orientationToPos(dir);

		if (!map_.isInside(pos))
		{
			result.emplace(iterator, wall, pos);
			continue;
		}

		result.emplace(iterator, map_.getTile(pos));
	}

	return result;
}

void AgentController::onAgentFail(AgentFailureType type, std::string message)
{
	std::cout << "AGENT FAILURE" << failureToString(type) << " " << message; // TODO: print it in the trace too
}