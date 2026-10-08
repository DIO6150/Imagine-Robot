#pragma once

#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/Map.hpp>

#include <Robot/Common/AgentCommandListener.hpp>
#include <Robot/Common/PlaygroundViewListener.hpp>

#include <Robot/Agent/Agent.hpp>

#include <Robot/Render/PlaygroundView.hpp>

class Playground : public AgentCommandListener, public PlaygroundViewListener
{
public:
	Playground();

	// Presenter Code
	void start(Directive const & instructions);
	void tick();
	void draw();

	// Agent Listener Code
	CommandResult<void> move(Orientation direction) override;
	
	CommandResult<void> consult() override;
	CommandResult<void> search(Orientation direction) override;

	CommandResult<Item> take(std::string name) override;
	CommandResult<void> give(Item item) override;


	CommandResult<Tile> see(Pos2D pos) override;

	void onAgentFail(AgentFailureType reason, std::string message);

	void wait() override;

private:
	Map map_;
	std::unique_ptr<Agent> agent_;
	std::unique_ptr<PlaygroundView> view_;
};