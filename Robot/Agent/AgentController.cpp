
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

CommandResult<void> AgentController::placeBackItem(Pos2D closet)
{
	listener_->placeBackItem(*carriedItem_);
	listener_->onAgentGiveItem(carriedItem_.value(), closet);
	carriedItem_ = std::nullopt;
	return CommandStatus::Ok;
}

CommandResult<std::vector<Tile>> AgentController::see()
{
	std::vector<Tile> perception = listener_->see();
	return perception;
}

CommandResult<std::string> AgentController::openCurrentDrawer()
{
	if(isChestNearby())
	{
		auto result = listener_->openCurrentDrawer(casier_);
		return result;
	}
	return CommandStatus::GodSaidNo;
}


CommandResult<void> AgentController::move(Orientation direction)
{
	Pos2D pos = pos_ + directionToPos(direction);

	if(!listener_->canMove()) return CommandStatus::AgentMovementObstructed;

	bool before = isChestNearby();
	auto old = pos_;
	pos_ = pos;
	bool after = isChestNearby();
	
	//if (before && !after) listener_->onAgentStashClose();
	listener_->onAgentMove(old, pos); // we close before moving and we open after moving
	//if (!before && after) listener_->onAgentStashOpen();

	return CommandStatus::Ok;
}

CommandResult<std::tuple<Emotion, Intensity>> AgentController::consult()
{
	std::tuple<Emotion, Intensity> result = listener_->consult(brain_->getCurrentMessage());
	listener_->onAgentMove(pos_, pos_);

	return result;
}

CommandResult<void> AgentController::search(Orientation direction)
{
	auto old = casier_;
	moveDrawerSelect(casier_, direction);
	listener_->onAgentSelect(old, casier_);
	return CommandStatus::Ok;
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

CommandResult<void> AgentController::give(Pos2D resident)
{
	if (carriedItem_)
	{
		if (!isPersonNearby()) return CommandStatus::ResidentNotInRange;

		listener_->onAgentGiveItem(carriedItem_.value(), resident);
		carriedItem_ = std::nullopt;
		return CommandStatus::Ok;
	}

	return CommandStatus::GodSaidNo;
}

CommandResult<void> AgentController::wait()
{
	return CommandStatus::Ok;
}

void AgentController::onAgentFail(AgentFailureType type, std::string message)
{
	std::cout << "AGENT FAILURE : " << failureToString(type) << " " << message << "\n"; // TODO: print it in the trace too
}

bool AgentController::isChestNearby()
{
	auto tiles = listener_->see();
	for (auto tile : tiles) if (tile.isItemPickup()) return true;
	return false;
}

bool AgentController::isPersonNearby()
{
	auto tiles = listener_->see();
	for (auto tile : tiles) if (tile.isPerson()) return true;
	return false;
}
