
#include <Robot/Simulation/Simulation.hpp>

DirectiveTrace Simulation::executeDirective(Directive instructions)
{
	playground_.start(instructions);
	std::cout << "Script, environnement and agent ready." << std::endl;
	
	while(!playground_.agentIsOver())
	{
		playground_.tick();
	}
	
	
	return DirectiveTrace {};
}

