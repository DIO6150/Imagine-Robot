#include <Robot/Simulation/Simulation.hpp>

int main(int argc, char ** argv)
{
	// load with command args here instead
	Directive instructions;
	instructions.map    = "data/cartes/appartement_01.json";
	instructions.script = "data/cartes/scenario_01.json";
	instructions.data   = "data/donnees";
	
	Simulation sim;
	sim.executeDirective(instructions);

	return 0;
}