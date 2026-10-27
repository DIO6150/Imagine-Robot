
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

std::vector<Pas> AgentController::returnSteps()
{
	return brain_->getSteps();
}

std::vector<Livraison> AgentController::returnDeliveries()
{
	return brain_->getDeliveries();
}

void AgentController::moveDrawerSelect(Pos2D & givenDrawer, Orientation direction)
{
	if (givenDrawer.l == 2 && direction == Orientation::South)
		std::cout << "Casier en intensite maximale, pas de deplacement" << std::endl;
	else if (givenDrawer.l == 0 && direction == Orientation::North)
		std::cout << "Casier en intensite minimale, pas de deplacement" << std::endl;
	else
	{
		givenDrawer = givenDrawer + directionToPos(direction);
		givenDrawer.c = (givenDrawer.c + 8) % 8;
	}
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

CommandResult<std::string> AgentController::search(Orientation direction)
{
	moveDrawerSelect(casier_, direction);
	std::string result = listener_->search(casier_);

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
