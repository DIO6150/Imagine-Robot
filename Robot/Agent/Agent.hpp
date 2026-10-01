#pragma once

#include <Robot/Agent/PlaygroundInterface.hpp>

class Agent
{
public:
	void Init(std::weak_ptr<PlaygroundInterface> playground);

private:
	std::weak_ptr<PlaygroundInterface> playground_;
};
