
#include <Robot/Simulation/Simulation.hpp>

using json = nlohmann::json;

DirectiveTrace Simulation::executeDirective(Directive instructions)
{
	playground_.start(instructions);

	while(1)
	{
		// if we have multiple SIMULATIONS MY NAMING IS WRONG AAAAAAAAAA
		// we would iterate trough and call tick() on each of them
		playground_.tick();
	}
	
	return DirectiveTrace {};
}
