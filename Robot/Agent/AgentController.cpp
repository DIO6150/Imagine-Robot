
#include <Robot/Agent/AgentController.hpp>


AgentController::AgentController(PlaygroundAgentListener * listener)
	: listener_ {listener}
{
	brain_ = std::make_unique<Brain>(this);
}

Pos2D AgentController::getPosition()
{
	return pos_;
}

void AgentController::start(std::vector<Request> requestList, MapInfo map, Pos2D casierDepart)
{
	MapInfo info = map;
	pos_ = info.robotStartPos;
	casier_ = casierDepart;

	brain_->start(requestList, info, casierDepart);
}

void AgentController::tick()
{
	brain_->tick();
}

bool AgentController::requestsOver()
{
	return brain_->noRequestsLeft();
}

CommandResult<std::vector<Tile>> AgentController::see()
{
	std::vector<Tile> perception = listener_->see();
	return perception;
}

CommandResult<void> AgentController::move(Orientation direction)
{
	Pos2D pos = pos_ + directionToPos(direction);

	if(!listener_->canMove())
		return CommandStatus::AgentMovementObstructed;

	pos_ = pos;

	return CommandStatus::Ok;
}

CommandResult<std::tuple<Emotion, Intensity>> AgentController::consult()
{
	std::tuple<Emotion, Intensity> result = listener_->consult(brain_->getCurrentMessage());

	if(get<0>(result) == Emotion::Unidentified || get<1>(result) == Intensity::Unidentified)
	{
		return result;
	}
	return result;
}

CommandResult<int> AgentController::search(Orientation direction)
{
	casier_ = casier_ + directionToPos(direction);
	casier_.x = casier_.x % 8;
	auto result = listener_->search(casier_);

	return result;
}

CommandResult<void> AgentController::take(std::string itemName)
{
	if (carriedItem_)
	{
		return CommandStatus::AgentAlreadyCarryingObject;
	}

	if (listener_->take(casier_, itemName))
	{
		carriedItem_ = itemName;
		return CommandStatus::Ok;
	}

	carriedItem_ = std::nullopt;
	return CommandStatus::GodSaidNo; // TODO: true error needed here
}

CommandResult<void> AgentController::give(std::string itemName)
{
	return CommandStatus::Ok;
}

void AgentController::onAgentFail(AgentFailureType type, std::string message)
{
	std::cout << "AGENT FAILURE : " << failureToString(type) << " " << message << "\n"; // TODO: print it in the trace too
}
