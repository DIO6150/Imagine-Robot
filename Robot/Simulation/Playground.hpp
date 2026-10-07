#pragma once

#include <Robot/Simulation/Dictionary.hpp>
#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/ItemStash.hpp>
#include <Robot/Simulation/Map.hpp>
#include <Robot/Simulation/Script.hpp>

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

	CommandResult<void> take(std::string name) override;
	CommandResult<void> give() override;


	CommandResult<AgentTileView> see() override;

	void wait() override;

private:
	Script script_;
	Map map_;
	ItemStash closet_;
	Dictionary dictionary_;
	std::unique_ptr<Agent> agent_;
	std::unique_ptr<PlaygroundView> view_;
};