
#include <Robot/Simulation/Simulation.hpp>

using json = nlohmann::json;

Simulation::Simulation()
{
	agent_ = std::make_unique<Agent>(this);
}

Script Simulation::getScript()
{
	return script_;
}

DirectiveTrace Simulation::executeDirective(Directive instructions)
{
	std::ifstream scriptStream {instructions.script};
	json scriptData = json::parse(scriptStream);
	auto status = script_.parseJSON(scriptData);
	
	if (status != JSONParserStatus::Ok)
		return 1;

	RecapMap recap = playground_.initalizePlayground(instructions);

	agent.initalizeAgent(recap);

	for(Request request : script_.getRequests())
	{
		agent_->tick();
	}

	while(1)
	{
		// if we have multiple SIMULATIONS MY NAMING IS WRONG AAAAAAAAAA
		// we would iterate trough and call tick() on each of them
		playground_.tick();
	//}
	
	return DirectiveTrace {};
}

void Simulation::tick()
{
	agent_->tick();
}
