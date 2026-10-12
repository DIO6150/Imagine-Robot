
#include <Robot/Simulation/Simulation.hpp>

DirectiveTrace Simulation::executeDirective(Directive instructions)
{
	playground_.start(instructions);

	
	while(!playground_.agentIsOver())
	{
		playground_.tick();
	}
	
	
	return DirectiveTrace {};
}

