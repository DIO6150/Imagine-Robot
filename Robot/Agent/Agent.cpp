
#include <Robot/Agent/Agent.hpp>

Agent::Agent(AgentCommandListener * listener)
	: listener_ {listener}
{

}

void Agent::start(std::initializer_list<Request> requests)
{

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
