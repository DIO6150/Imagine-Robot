#pragma once

#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/Playground.hpp>
#include <Robot/Agent/Agent.hpp>

#include <Robot/Errors/Error.hpp>

class Simulation : public AgentCommandListener
{
public:
	Simulation();
	DirectiveTrace executeDirective(Directive instructions);
	Script getScript();
	
	void setCurRequestEnd(bool state);

	CommandResult<void> move(Orientation direction) override;
	
	CommandResult<void> consult() override;
	CommandResult<void> search(Orientation direction) override;

	CommandResult<Item> take(std::string name) override;
	CommandResult<void> give(Item item) override;


	CommandResult<Tile> see(Pos2D pos) override;

	void onAgentFail(AgentFailureType reason, std::string message);

	void wait() override;

private:
	Playground playground_;
	std::unique_ptr<Agent> agent_;
	Script script_;
	bool curRequestEnd_;
};

