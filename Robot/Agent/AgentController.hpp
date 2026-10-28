#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <Robot/Common/Request.hpp>
#include <Robot/Common/PlaygroundAgentListener.hpp>
#include <Robot/Common/MapInfo.hpp>
#include <Robot/Common/TraceElement.hpp>
#include <Robot/Agent/AgentControllerListener.hpp>
#include <Robot/Agent/Brain.hpp>

#include <Robot/Utils/Pos.hpp>

class Playground;
class Brain;

class AgentController: public AgentControllerListener
{
public:
	AgentController(PlaygroundAgentListener * listener);
	Pos2D getPosition() const;

	void start(std::vector<Request> requestList, MapInfo map, Pos2D casierDepart);
	void tick();
	bool requestsOver();

	std::vector<Pas> returnSteps();
	std::vector<Livraison> returnDeliveries();

	static void moveDrawerSelect(Pos2D & casier, Orientation direction);
private:

	CommandResult<std::vector<Tile>> see() override;
	CommandResult<void> move(Orientation direction) override;
	
	CommandResult<std::tuple<Emotion, Intensity>> consult() override;
	CommandResult<std::optional<Item>> search(Orientation direction) override;

	CommandResult<Item> take() override;
	CommandResult<void> give() override;

	void onAgentFail(AgentFailureType reason, std::string message) override;

private:
	Pos2D pos_;
	Pos2D casier_;

	std::optional<Item> carriedItem_ {std::nullopt};

private:
	std::unique_ptr<Brain> brain_;
	PlaygroundAgentListener * listener_;
};

