#include <string>
#include <Robot/Simulation/Simulation.hpp>

int main(int argc, char ** argv)
{
	// load with command args here instead
	Directive instructions;
	instructions.map             = "data/cartes/appartement_01.json";
	instructions.script          = "data/cartes/scenario_01.json";
	std::string chemin_donnees   = "data/donnees";
	instructions.closet          = chemin_donnees + "/armoire_standard.json";
	instructions.dictionary      = chemin_donnees + "/dictionnaire.json";
	
	Simulation sim;
	auto result = sim.executeDirective(instructions);

	// do something with the trace

	return 0;
}