
#include <Robot/Agent/AgentController.hpp>


AgentController::AgentController(PlaygroundAgentListener * listener)
	: listener_ {listener}
{
	brain_ = std::make_unique<Brain>(this);
}

Pos2D AgentController::getPosition() const
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

	listener_->onAgentMove(pos_, pos);
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

CommandResult<std::optional<Item>> AgentController::search(Orientation direction)
{
	moveDrawerSelect(casier_, direction);
	auto result = listener_->search(casier_);

	if (result) return result;
	return CommandStatus::GodSaidNo;
}

CommandResult<Item> AgentController::take()
{
	if (carriedItem_)
	{
		return CommandStatus::AgentAlreadyCarryingObject;
	}

	carriedItem_ = listener_->take(casier_);
	
	if (carriedItem_) return carriedItem_.value();
	return CommandStatus::GodSaidNo;
}

CommandResult<void> AgentController::give()
{
	if (carriedItem_)
	{
		// TODO : check if person is in reach

		listener_->onAgentGiveItem(carriedItem_.value());
		carriedItem_ = std::nullopt;
		return CommandStatus::Ok;
	}

	return CommandStatus::GodSaidNo;
}

void AgentController::onAgentFail(AgentFailureType type, std::string message)
{
	std::cout << "AGENT FAILURE : " << failureToString(type) << " " << message << "\n"; // TODO: print it in the trace too
}
