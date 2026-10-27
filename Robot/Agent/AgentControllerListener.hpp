#pragma once

#include <vector>
#include <tuple>

#include <Robot/Errors/Error.hpp>
#include <Robot/Common/Orientation.hpp>
#include <Robot/Common/Tile.hpp>
#include <Robot/Common/Item.hpp>

struct AgentControllerListener
{
	virtual CommandResult<void> move(Orientation direction) = 0;

	virtual CommandResult<std::tuple<Emotion, Intensity>> consult() = 0;
	virtual CommandResult<std::string> search(Orientation direction) = 0;

	virtual CommandResult<void> take(std::string name) = 0;
	virtual CommandResult<void> give(std::string name) = 0;

	virtual CommandResult<std::vector<Tile>> see() = 0;

	virtual void onAgentFail(AgentFailureType type, std::string message) = 0;
};
