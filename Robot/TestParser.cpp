#include <string>
#include <Robot/Simulation/Simulation.hpp>

int main(int argc, char ** argv)
{
	if(argc != 2){
        std::cout << "Arguments attendus : type ('scenario', 'carte', 'armoire', 'dictionnaire'" << std::endl;
        return 0;
    }

    Playground playground;
    JSONParserStatus status = playground.dataParser(argv[2], argv[1]);

    if(argv[1] == "scenario")
        playground.getScript().scriptInfos();
    else if(argv[1] == "carte")
        playground.getMap().mapInfos();
    else if(argv[1] == "armoire")
        playground.getCloset().closetInfos();
    else if(argv[1] == "dictionnaire")
        playground.getDictionary().dictionaryInfos();
    else
        std::cout << "Pas de destination de remplissage." << std::endl;
    
    if (status != JSONParserStatus::Ok)
        return 1;
	return 0;
}