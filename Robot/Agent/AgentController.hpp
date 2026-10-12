#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <Robot/Common/Request.hpp>
#include <Robot/Agent/AgentControllerListener.hpp>
#include <Robot/Agent/Brain.hpp>
#include <Robot/Simulation/Playground.hpp>

#include <Robot/Utils/Pos.hpp>

class Playground;
class Brain;

class AgentController: public AgentControllerListener
{
public:
	AgentController(Playground * playground);
	Pos2D getPosition();

	void start(std::vector<Request> requestList, MapInfo map, Pos2D casierDepart);
	void tick();
	bool requestsOver();

private:

	CommandResult<std::vector<Tile>> see() override;
	CommandResult<void> move(Orientation direction) override;
	
	CommandResult<std::tuple<Emotion, Intensity>> consult() override;
	CommandResult<int> search(Orientation direction) override;

	CommandResult<void> take(std::string itemName) override;
	CommandResult<void> give(std::string itemName) override;

	void onAgentFail(AgentFailureType reason, std::string message) override;

private:
	Pos2D pos_;
	Pos2D casier_;

	std::optional<Item> carriedItem_ {std::nullopt};

private:
	std::unique_ptr<Brain> brain_;
	Playground * playground_;
};

