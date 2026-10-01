#pragma once

#include <Robot/Common/AgentCommandListener.hpp>

struct Request
{
	uint32_t id_;
	std::string resident_;
	std::string message_;
};

class Agent
{
public:
	Agent(AgentCommandListener * listener);

	void start(std::initializer_list<Request> requests);
	void tick();

	Pos2D getPosition() const;
	void setPosition(Pos2D newPos);

private:
	Pos2D pos_;

private:
	AgentCommandListener * listener_;
};