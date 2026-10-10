#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <Robot/Common/Request.hpp>

#include <Robot/Agent/AgentControllerListener.hpp>
#include <Robot/Agent/Brain.hpp>

#include <Robot/Utils/Pos.hpp>


class Map;

class AgentController: public AgentControllerListener
{
public:
	AgentController(Map const & map);

	void start(std::vector<Request> requestList);
	void tick();

private:
	CommandResult<void> move(Orientation direction) override;
	
	CommandResult<void> consult() override;
	CommandResult<void> search(Orientation direction) override;

	CommandResult<Item> take(std::string name) override;
	CommandResult<void> give() override;

	CommandResult<std::vector<Tile>> see() override;

	void onAgentFail(AgentFailureType reason, std::string message) override;

private:
	Pos2D pos_;

	std::optional<Item> carriedItem_ {std::nullopt};

private:
	std::unique_ptr<Brain> brain_;
	Map const & map_;
};
