
#include <Robot/Agent/Agent.hpp>

Agent::Agent(AgentCommandListener * listener)
	: listener_ {listener}
{

}

void Agent::start(std::vector<Request> list_requests)
{
	requests_ = list_requests;
}

void Agent::tick()
{

}

Pos2D Agent::getPosition() const
{
	return pos_;
}

void Agent::setPosition(Pos2D newPos)
{
	pos_ = newPos;
}

Request Agent::getCurrentRequest()
{
	return requests_.front();
}

void Agent::removeRemoveRequest()
{
	requests_.erase(requests_.begin());
}

void Agent::initalizeMentalMap(int height, int width)
{
	std::vector<std::tuple<Tile, int>> MentalMap;
}