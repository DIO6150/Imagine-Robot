
#include <Robot/Agent/AgentController.hpp>


AgentController::AgentController(Playground * playground)
	: playground_ {playground}
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
	std::vector<Tile> perception = playground_->agentSee();
	return CommandResult{CommandStatus::Ok, perception};
}

CommandResult<void> AgentController::move(Orientation direction)
{
	Pos2D pos = pos_ + directionToPos(direction);

	if(!playground_->agentCanMove())
		return CommandStatus::AgentMovementObstructed;

	pos_ = pos;

	return CommandStatus::Ok;
}

CommandResult<std::tuple<Emotion, Intensity>> AgentController::consult()
{
	std::tuple<Emotion, Intensity> result = playground_->agentConsult(brain_->getCurrentMessage());
	if(get<0>(result) == Emotion::Unidentified || get<1>(result) == Intensity::Unidentified) {
		return CommandResult{CommandStatus::UnindentifiedEmotion, result};
	}
	return CommandResult{CommandStatus::Ok, result};
}

CommandResult<int> AgentController::search(Orientation direction)
{
	casier_ = casier_ + directionToPos(direction);
	casier_.x = casier_.x%8;
	auto result = playground_->agentSearch(casier_);
	return CommandResult{CommandStatus::Ok, result};
}

CommandResult<void> AgentController::take(std::string itemName)
{
	playground_->agentTake(casier_, itemName);
	return CommandStatus::Ok;
}

CommandResult<void> AgentController::give(std::string itemName)
{
	
	return CommandStatus::Ok;
}

void AgentController::onAgentFail(AgentFailureType type, std::string message)
{
	std::cout << "AGENT FAILURE" << failureToString(type) << " " << message; // TODO: print it in the trace too
}
