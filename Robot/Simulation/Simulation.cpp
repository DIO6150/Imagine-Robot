
#include <Robot/Simulation/Simulation.hpp>

DirectiveTrace Simulation::executeDirective(Directive instructions)
{
	playground_.start(instructions);

	//while(1)
	//{
		playground_.tick();
	//}
	
	return DirectiveTrace {};
}
