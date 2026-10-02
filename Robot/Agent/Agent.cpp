
#include <Robot/Agent/Agent.hpp>

Agent::Agent(AgentCommandListener * listener)
	: listener_ {listener}
{

}

void Agent::start(std::initializer_list<Request> list_requests)
{
	requests_.fill(requests_.end(), list_requests.begin(), list_requests.end());
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

Request Agent::getRequest(int index) const
{
	return requests.front();
}

void removeRemoveRequest()
{
	requests.erase(requests.begin());
}

void initalizeMentalMap(int height, int width)
{
	std::vector<std::tuple<Tile, int>> MentalMap;
}