#include <string>
#include <fstream>
#include <Robot/Simulation/Simulation.hpp>
#include <vendor/nlohmann/json.hpp>

using ordered_json = nlohmann::ordered_json;
using json = nlohmann::json;

void testPlanification()
{
	std::cout << "Tests de planification de chemin :\n" << std::endl;

	int nlines = 7, ncolumns = 10;
	std::vector<Tile> reducedMap = std::vector<Tile>(10*7);
	TileProperty empty;
	Pos2D pos;
	for(pos.l = 0; pos.l < nlines; ++pos.l)
	{
		for (pos.c = 0; pos.c < ncolumns; ++pos.c)
		{
			auto index = pos.l * ncolumns + pos.c;

			reducedMap.at(index) = Tile {empty, pos};

			bool inside = (pos.l < 1 || pos.l >= nlines - 1 || pos.c < 1 || pos.c >= ncolumns - 1);
			bool wallRes = ((pos.l == 2 && pos.c <= 5 ) || pos == Pos2D{3,5} || pos == Pos2D{4,5} || pos == Pos2D{4,6} || pos == Pos2D{4,7});
			if (inside || wallRes)
			{
				reducedMap[index].setSolid(true);
			}
		}
	}
	reducedMap[12].setAgentStart(true);
	reducedMap[27].setSolid(true).setPerson(true);
	std::cout << "Tests de destination inaccessible :\n" << std::endl;
	std::cout << "Carte mentale:" << std::endl;
	std::cout << "##########\n#.R......#\n######...#\n#.P..#...#\n";
	std::cout << "#....###.#\n#........#\n##########\n" << std::endl;
	std::queue chemin = Brain::shortestPath(Pos2D {1, 2}, Pos2D {3, 2}, reducedMap, ncolumns);
	std::cout << "Chemin trouve : ";
	for (int i = 0; i < chemin.size(); i++) {
		Pos2D pos = chemin.front();
		chemin.pop();
		std::cout << "(" << pos.l << "," << pos.c << ") ";
		chemin.push(pos);
	}
	std::cout << "\n" << std::endl;
}

void testReplanification()
{
	std::cout << "Tests de replanification de chemin :" << std::endl;
}

void singleTestLectureDictionnaire(Dictionary dictioTest, std::string messageTest, int emoAtt, int intensAtt)
{
	std::cout << "Message d'entree : " << messageTest << std::endl;
	std::cout << "Emotion et intensite attendues : (" << emoAtt << "," << intensAtt << ")" << std::endl;
	std::tuple<Emotion, Intensity> retour = dictioTest.identifEmoIntens(messageTest);
	int emotionRetour = emotionToInt(get<0>(retour));
	int intensityRetour = intensityToInt(get<1>(retour));
	std::cout << "Emotion et intensite identifiees : (" << emotionRetour << ",";
	std::cout << intensityRetour << ")\n" << std::endl;
}

void testLectureDictionnaire()
{
	Dictionary dictioTest;
	JSONParserStatus status = Playground::parse<Dictionary>("data/donnees/dictionnaire.json", dictioTest);
	std::cout << "Tests de la lecture du message dans le dictionnaire :\n" << std::endl;
	
	singleTestLectureDictionnaire(dictioTest, "Te voir me donne le sourire.", 0, 1);
	singleTestLectureDictionnaire(dictioTest, "J'accepte ma destinée.", 1, 0);
	singleTestLectureDictionnaire(dictioTest, "Je suis terrifie.", 2, 2);
	singleTestLectureDictionnaire(dictioTest, "N'y a t'il pas quelque chose d'etrange ici?", 3, 0);
	singleTestLectureDictionnaire(dictioTest, "Je nage dans le desespoir...", 4, 2);
	singleTestLectureDictionnaire(dictioTest, "J'ai mange quelque chose de fade ce midi.", 5, 0);
	singleTestLectureDictionnaire(dictioTest, "C'est scandaleux de ne pas pouvoir respirer!", 6, 2);
	singleTestLectureDictionnaire(dictioTest, "Je suis impatiente d'en finir avec ce projet...", 7, 1);
	singleTestLectureDictionnaire(dictioTest, "Je suis calme, et certainement pas en panique.", 0, 0);
	singleTestLectureDictionnaire(dictioTest, "Ne cherchez pas d'emotion dans ce messgage, il n'y en a pas", -1, -1);
}

void testDeplacementSelecteur()
{
	std::cout << "Tests de deplacement du curseur dans l'armoire :\n" << std::endl;
	Pos2D casierTest = Pos2D {0, 0};
	std::cout << "Casier en position (intensite = " << casierTest.l << ", emotion = " << casierTest.c << ")" << std::endl;
	std::cout << "Deplacement Nord : ";
	AgentController::moveDrawerSelect(casierTest, Orientation::North);
	std::cout << "Casier en position (intensite = " << casierTest.l << ", emotion = " << casierTest.c << ")" << std::endl;
	std::cout << "Deplacement Ouest : ";
	AgentController::moveDrawerSelect(casierTest, Orientation::West);
	std::cout << "Casier en position (intensite = " << casierTest.l << ", emotion = " << casierTest.c << ")" << std::endl;
	std::cout << "Deplacement Sud : ";
	AgentController::moveDrawerSelect(casierTest, Orientation::South);
	std::cout << "Casier en position (intensite = " << casierTest.l << ", emotion = " << casierTest.c << ")" << std::endl;
	std::cout << "Deplacement Est : ";
	AgentController::moveDrawerSelect(casierTest, Orientation::East);
	std::cout << "Casier en position (intensite = " << casierTest.l << ", emotion = " << casierTest.c << ")" << std::endl;
	std::cout << "Deplacement Sud : ";
	AgentController::moveDrawerSelect(casierTest, Orientation::South);
	std::cout << "Casier en position (intensite = " << casierTest.l << ", emotion = " << casierTest.c << ")" << std::endl;
	std::cout << "Deplacement Sud : ";
	AgentController::moveDrawerSelect(casierTest, Orientation::South);
	std::cout << "Casier en position (intensite = " << casierTest.l << ", emotion = " << casierTest.c << ")\n" << std::endl;
}

void testRegleRepli()
{
	std::cout << "Tests de la regle de repli :" << std::endl;
}

void testCaseInccessible()
{
	int nlines = 7, ncolumns = 10;
	std::vector<Tile> reducedMap = std::vector<Tile>(10*7);
	TileProperty empty;
	Pos2D pos;
	for(pos.l = 0; pos.l < nlines; ++pos.l)
	{
		for (pos.c = 0; pos.c < ncolumns; ++pos.c)
		{
			auto index = pos.l * ncolumns + pos.c;

			reducedMap.at(index) = Tile {empty, pos};

			bool inside = (pos.l < 1 || pos.l >= nlines - 1 || pos.c < 1 || pos.c >= ncolumns - 1);
			bool wallRes = ((pos.l <= 4 && pos.c == 5 ) || (pos.l == 4 && pos.c > 5));
			if (inside || wallRes)
			{
				reducedMap[index].setSolid(true);
			}
		}
	}
	reducedMap[12].setAgentStart(true);
	reducedMap[27].setSolid(true).setPerson(true);
	std::cout << "Tests de destination inaccessible :\n" << std::endl;
	std::cout << "Carte mentale :" << std::endl;
	std::cout << "##########\n#.R..#...#\n#....#.P.#\n#....#...#\n";
	std::cout << "#....#####\n#........#\n##########\n" << std::endl;
	std::queue chemin = Brain::shortestPath(Pos2D {1, 2}, Pos2D {2, 7}, reducedMap, ncolumns);
	std::cout << "L'agent peut rejoindre le resident ? ";
	std::cout << ((chemin.empty()) ? "non" : "oui") << "\n" << std::endl;
}

void testValidationFichiers()
{
	std::cout << "Tests de validation des fichiers :" << std::endl;
}

void testOne(std::string testType)
{
	if (testType == "tests_plan")
		testPlanification();
	else if (testType == "tests_replan")
		testReplanification();
	else if (testType == "tests_lecture")
		testLectureDictionnaire();
	else if (testType == "tests_select")
		testDeplacementSelecteur();
	else if (testType == "tests_repli")
		testRegleRepli();
	else if (testType == "tests_acces")
		testCaseInccessible();	
	else if (testType == "tests_fichier")
		testValidationFichiers();
	else if (testType == "tests_tous")
	{
		testPlanification();
		testReplanification();
		testLectureDictionnaire();
		testDeplacementSelecteur();
		testRegleRepli();
		testCaseInccessible();
		testValidationFichiers();
	}
	else
		std::cout << "Argument de test invalide." << std::endl; 

	std::cout << "Execution des tests terminee." << std::endl;
}

int main(int argc, char ** argv)
{
	switch (argc)
	{
		case 2 :
			testOne(argv[1]);
			return 0;
		case 1 :
			std::cout << "Lancement de la simulation." << std::endl;
			break;
		default :		
			std::cout << "Lancer la simulation :" << std::endl;
			std::cout << "- argument 1 : chemin de la carte" << std::endl;
			std::cout << "- argument 2 : chemin du scenario" << std::endl;
			std::cout << "- argument 3 : chemin du dossier de donnees" << std::endl;
			std::cout << "- argument 4 : chemin de la trace a ecrire" << std::endl;
			std::cout << "\nLancer les tests :" << std::endl;
			std::cout << "- argument 1 : tests_tous || tests_plan || tests_replan || tests_lecture";
			std::cout << " || tests_select || tests_repli || tests_acces || tests_fichier" << std::endl;
			return 0;
	}
	
	// load with command args here instead
	Directive instructions;
	instructions.map    = "data/cartes/appartement_01.json";
	instructions.script = "data/cartes/scenario_01.json";
	instructions.data   = "data/donnees";
	std::string traceFileName  = "trace.json";
	
	Simulation sim;
	DirectiveTrace trace;
	JSONParserStatus statusSim = sim.executeDirective(instructions, trace);

	if (statusSim != JSONParserStatus::Ok)
	{
		printError(statusSim);
		return static_cast<int>(statusSim);
	}

	std::cout << "Simulation terminee sans erreur de chargement." << std::endl;

	// do something with the trace
	std::ofstream traceFile(traceFileName);
	if (!traceFile.is_open()) {
        std::cout << "failed to open " << traceFileName << std::endl;
		return 1;
	}
	ordered_json dataTrace;

	dataTrace["format"] = trace.format;
	dataTrace["version"] = trace.version;
	dataTrace["carte"] = trace.carte;
	dataTrace["scenario"] = trace.scenario;
	dataTrace["equipe"] = trace.equipe;

	dataTrace["pas"] = json::array();

	ordered_json jpas, jpas_perc;
	for (Pas pas : trace.pas)
	{
		jpas["t"] = pas.t;
		jpas["demande"] = pas.demande;
		jpas["position"] = {pas.position.l, pas.position.c};
		jpas["casier"] = {pas.casier.l, pas.casier.c};
		jpas["action"] = pas.action;
		jpas["argument"] = ((pas.argument == "null") ? json(nullptr) : json(pas.argument));
		jpas_perc["N"] = pas.perception["N"];
		jpas_perc["S"] = pas.perception["S"];
		jpas_perc["E"] = pas.perception["E"];
		jpas_perc["O"] = pas.perception["W"];
		jpas["perception"] = jpas_perc;
		jpas["contenu_casier"] = ((pas.contenu_casier == "null") ? json(nullptr) : json(pas.contenu_casier));
		jpas["commentaire"] = ((pas.commentaire == "null") ? json(nullptr) : json(pas.commentaire));
		dataTrace["pas"].push_back(jpas);
	}

	ordered_json jliv;
	dataTrace["livraison"] = json::array();
	int nbSucces = 0;
	for (Livraison livraison : trace.livraisons)
	{
		jliv["demande"] = livraison.demande;
		jliv["resident"] = livraison.resident;
		jliv["emotion"] = livraison.emotion;
		jliv["intensite"] = livraison.intensity;
		jliv["casier_choisi"] = {livraison.casier_choisi.l, livraison.casier_choisi.c};
		jliv["repli"] = livraison.repli;
		jliv["objet"] = ((livraison.objet == "null") ? json(nullptr) : json(livraison.objet));
		jliv["succes"] = livraison.succes;
		if (livraison.succes)
			nbSucces++;
		jliv["motif_echec"] = ((livraison.motif_echec == "null") ? json(nullptr) : json(livraison.motif_echec));
		dataTrace["livraison"].push_back(jliv);
	}

	dataTrace["resume"]["demandes"] = trace.livraisons.size();
	dataTrace["resume"]["reussies"] = nbSucces;
	dataTrace["resume"]["echecs"] = trace.livraisons.size() - nbSucces;
	dataTrace["resume"]["pas_total"] = trace.pas[trace.pas.size()-1].t + 1;

	traceFile << dataTrace.dump(4);

	traceFile.close();

	return 0;
}