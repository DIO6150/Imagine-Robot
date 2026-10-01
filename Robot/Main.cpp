#include <Robot/Simulation/Simulation.hpp>

int main(int argc, char ** argv)
{
	Directive instructions;
	instructions.map = "data/cartes/appartement_01.json";
	
	Simulation sim;
	sim.executeDirective(instructions);

	return 0;
}