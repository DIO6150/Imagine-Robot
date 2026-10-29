#include <string>
#include <fstream>
#include <filesystem>
#include <Robot/Simulation/Simulation.hpp>
#include <vendor/nlohmann/json.hpp>

using ordered_json = nlohmann::ordered_json;
using json = nlohmann::json;

std::vector<Tile> initReducedMap(Pos2D robotPos, Pos2D resPos, std::vector<Pos2D> extraWalls)
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
			bool isWall = (inside || count(extraWalls.begin(), extraWalls.end(), pos) > 0);
			reducedMap[index].setSolid(isWall);
		}
	}
	reducedMap[robotPos.l * 10 + robotPos.c].setAgentStart(true);
	reducedMap[resPos.l * 10 + resPos.c].setSolid(true).setPerson(true);
	return reducedMap;
}

void testPlanification()
{
	std::vector<Pos2D> testWalls = {Pos2D {2,1}, Pos2D {2,2}, Pos2D {2,3}, Pos2D {2,4}, Pos2D {3,4}, Pos2D {4,4}, Pos2D {4,5}, Pos2D {4,6}};
	std::vector<Tile> reducedMap = initReducedMap(Pos2D {1,2}, Pos2D {3,2}, testWalls);
	std::cout << "Carte mentale:" << std::endl;
	std::cout << "##########\n#.R......#\n#####....#\n#.P.#....#\n";
	std::cout << "#...###..#\n#........#\n##########\n" << std::endl;
	std::queue chemin = Brain::shortestPath(Pos2D {1,2}, Pos2D {3,2}, reducedMap, 10);
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

	std::vector<Pos2D> testWalls = {Pos2D {2,1}, Pos2D {2,2}, Pos2D {2,3}, Pos2D {2,4}};
	std::vector<Tile> reducedMap = initReducedMap(Pos2D {3,3}, Pos2D {3,1}, testWalls);
	std::cout << "Carte mentale avant perception :" << std::endl;
	std::cout << "##########\n#........#\n#####....#\n#.P..R...#\n";
	std::cout << "#........#\n#........#\n##########\n" << std::endl;
	std::queue chemin = Brain::shortestPath(Pos2D {3,5}, Pos2D {3,2}, reducedMap, 10);
	std::cout << "Chemin trouve : ";
	for (int i = 0; i < chemin.size(); i++) {
		Pos2D pos = chemin.front();
		chemin.pop();
		std::cout << "(" << pos.l << "," << pos.c << ") ";
		chemin.push(pos);
	}
	std::cout << "\n" << std::endl;

	reducedMap[34].setSolid(true);
	reducedMap[45].setSolid(true);

	std::cout << "Carte mentale apres perception :" << std::endl;
	std::cout << "##########\n#........#\n#####....#\n#.P.#R...#\n";
	std::cout << "#....#...#\n#........#\n##########\n" << std::endl;

	chemin = Brain::shortestPath(Pos2D {3,5}, Pos2D {3,2}, reducedMap, 10);
	std::cout << "Chemin trouve : ";
	for (int i = 0; i < chemin.size(); i++) {
		Pos2D pos = chemin.front();
		chemin.pop();
		std::cout << "(" << pos.l << "," << pos.c << ") ";
		chemin.push(pos);
	}
	std::cout << "\n" << std::endl;
}

void testCaseInccessible()
{
	std::vector<Pos2D> testWalls = {Pos2D {2,1}, Pos2D {2,2}, Pos2D {2,3}, Pos2D {2,4}, Pos2D {3,4}, Pos2D {4,4}, Pos2D {4,3}, Pos2D {4,2}, Pos2D {4,1}};
	std::vector<Tile> reducedMap = initReducedMap(Pos2D {3,5}, Pos2D {3,2}, testWalls);
	std::cout << "Tests de destination inaccessible :\n" << std::endl;
	std::cout << "Carte mentale :" << std::endl;
	std::cout << "##########\n#........#\n#####....#\n#.P.#R...#\n";
	std::cout << "#####....#\n#........#\n##########\n" << std::endl;
	std::queue chemin = Brain::shortestPath(Pos2D {3,5}, Pos2D {3,2}, reducedMap, 10);
	std::cout << "L'agent peut rejoindre le resident ? ";
	std::cout << ((chemin.empty()) ? "non" : "oui") << "\n" << std::endl;
}

void singleTestLectureDictionnaire(std::map<std::string, Entree> record, std::string messageTest, int emoAtt, int intensAtt)
{
	std::cout << "Message d'entree : " << messageTest << std::endl;
	std::cout << "Emotion attendue : " << emoAtt << ", intensite attendue : " << intensAtt << std::endl;
	std::tuple<Emotion, Intensity> retour = Dictionary::identifEmoIntens(messageTest, record);
	int emotionRetour = emotionToInt(get<0>(retour));
	int intensityRetour = intensityToInt(get<1>(retour));
	std::cout << "Emotion identifiee : " << emotionRetour << ", intensite identifiee : ";
	std::cout << intensityRetour << "\n" << std::endl;
}

void testLectureDictionnaire()
{
	std::map<std::string, Entree> reducedDictio;
	reducedDictio.emplace("sourire", Entree{Emotion::Joy, Intensity::Mid});
	reducedDictio.emplace("accepte", Entree{Emotion::Confidence, Intensity::Small});
	reducedDictio.emplace("terrifie", Entree{Emotion::Fear, Intensity::Strong});
	reducedDictio.emplace("etrange", Entree{Emotion::Surprised, Intensity::Small});
	reducedDictio.emplace("desespoir", Entree{Emotion::Sadness, Intensity::Strong});
	reducedDictio.emplace("fade", Entree{Emotion::Disgust, Intensity::Small});
	reducedDictio.emplace("scandaleux", Entree{Emotion::Anger, Intensity::Strong});
	reducedDictio.emplace("impatiente", Entree{Emotion::Anticipation, Intensity::Mid});
	reducedDictio.emplace("calme", Entree{Emotion::Joy, Intensity::Small});
	reducedDictio.emplace("panique", Entree{Emotion::Fear, Intensity::Strong});
	std::cout << "Tests de la lecture du message dans le dictionnaire :\n" << std::endl;

	std::cout << "Entrees du dictionnaire : " << std::endl;
	for (auto & entree : reducedDictio)
	{
		std::cout << "Mot : " << entree.first << ", emotion : " << emotionToInt(entree.second.emotion_);
		std::cout << ", intensite : " << intensityToInt(entree.second.intensity_) << std::endl;
	}
	std::cout << "\n" << std::endl;
	
	singleTestLectureDictionnaire(reducedDictio, "Te voir me donne le sourire.", 0, 1);
	singleTestLectureDictionnaire(reducedDictio, "J'accepte ma destinée.", 1, 0);
	singleTestLectureDictionnaire(reducedDictio, "Je suis terrifie.", 2, 2);
	singleTestLectureDictionnaire(reducedDictio, "N'y a t'il pas quelque chose d'etrange ici?", 3, 0);
	singleTestLectureDictionnaire(reducedDictio, "Je nage dans le desespoir...", 4, 2);
	singleTestLectureDictionnaire(reducedDictio, "J'ai mange quelque chose de fade ce midi.", 5, 0);
	singleTestLectureDictionnaire(reducedDictio, "C'est scandaleux de ne pas pouvoir respirer!", 6, 2);
	singleTestLectureDictionnaire(reducedDictio, "Je suis impatiente d'en finir avec ce projet...", 7, 1);
	singleTestLectureDictionnaire(reducedDictio, "Je suis calme, et certainement pas en panique.", 0, 0);
	singleTestLectureDictionnaire(reducedDictio, "Ne cherchez pas d'emotion dans ce messgage, il n'y en a pas", -1, -1);
}

void singleTestDeplacementSelecteur(Pos2D casierTest, int iOrient)
{
	std::vector<Orientation> dir = {North, West, South, East};
	std::vector<std::string> dirToString = {"Nord", "Ouest", "Sud", "Est"};
	std::cout << "Deplacement " << dirToString[iOrient] << " : ";
	AgentController::moveDrawerSelect(casierTest, dir[iOrient]);
	std::cout << "Casier en position (intensite = " << casierTest.l << ", emotion = " << casierTest.c << ")" << std::endl;
}

void testDeplacementSelecteur()
{
	std::cout << "Tests de deplacement du curseur dans l'armoire :\n" << std::endl;
	Pos2D casierTest = Pos2D {0, 0};
	std::cout << "Casier en position (intensite = " << casierTest.l << ", emotion = " << casierTest.c << ")" << std::endl;
	
	singleTestDeplacementSelecteur(casierTest, 0);
	singleTestDeplacementSelecteur(casierTest, 1);
	singleTestDeplacementSelecteur(casierTest, 2);
	singleTestDeplacementSelecteur(casierTest, 3);
	singleTestDeplacementSelecteur(casierTest, 2);
	singleTestDeplacementSelecteur(casierTest, 2);
}

void singleTestDeplacementSelecteur(Pos2D casierChoisi, Emotion emoRecherche, Intensity intensRecherche, std::string repliAtt)
{
	std::cout << "Casier fourni choisi : (intensite = " << casierChoisi.l << ", emotion = " << casierChoisi.c << ")" << std::endl;
	std::cout << "Repli attendu : " << repliAtt << std::endl;
	std::string repliRetour = Brain::determineRepli(casierChoisi, emoRecherche, intensRecherche);
	std::cout << "Repli determine : " << repliRetour << "\n" << std::endl;
}

void testRegleRepli()
{
	std::cout << "Tests de la regle de repli :\n" << std::endl;
	Emotion emoRecherche = Emotion::Joy;
	Intensity intensRecherche = Intensity::Strong;
	std::cout << "Emotion recherchee : " << emotionToInt(emoRecherche);
	std::cout << ", intensite recherchee : " << intensityToInt(intensRecherche) << std::endl;

	singleTestDeplacementSelecteur(Pos2D{2, 0}, emoRecherche, intensRecherche, "aucun");
	singleTestDeplacementSelecteur(Pos2D{0, 0}, emoRecherche, intensRecherche, "intensite");
	singleTestDeplacementSelecteur(Pos2D{2, 1}, emoRecherche, intensRecherche, "voisine_1");
	singleTestDeplacementSelecteur(Pos2D{1, 6}, emoRecherche, intensRecherche, "voisine_2");
	singleTestDeplacementSelecteur(Pos2D{1, 3}, emoRecherche, intensRecherche, "casiers_vides");
}

void testValidationFichiers(std::string folderTestPath)
{
	std::cout << "Tests de validation des fichiers :\n" << std::endl;

	Script testScript;
	Map testMap;
	ItemStash closetTest;
	Dictionary dictionaryTest;

	json dataToParse;
	int parsingType = 0;
	
	std::cout << "Test de fichier manquant : \n" << folderTestPath << "/dummytest.json" << std::endl;
	JSONParserStatus statusTest = Playground::extractAndDetermineParsing(folderTestPath + "/dummytest.json", parsingType, dataToParse);
	printError(statusTest);
	
	const std::filesystem::path testsFiles{folderTestPath};

	for (auto const& dir_entry : std::filesystem::directory_iterator{testsFiles})
	{
		std::cout << "Fichier : " << dir_entry.path() << std::endl;
		std::string strPath {dir_entry.path().string()};

		statusTest = Playground::extractAndDetermineParsing(strPath, parsingType, dataToParse);
		if (statusTest != JSONParserStatus::Ok)
		{
			printError(statusTest);
			continue;
		}

		//std::cout << "Type a parse : " << parsingType << std::endl;
		switch (parsingType)
		{
			case 0:
				statusTest = Playground::parse<Script>(dataToParse, testScript);
				break;
			case 1:
				statusTest = Playground::parse<Map>(dataToParse, testMap);
				break;
			case 2:
				statusTest = Playground::parse<ItemStash>(dataToParse, closetTest);
				break;
			case 3:
				statusTest = Playground::parse<Dictionary>(dataToParse, dictionaryTest);
				break;
			default:
				printError(JSONParserStatus::WrongFormat);
				continue;
		}
		printError(statusTest);
	}
	statusTest = Playground::conformScriptMap(testScript, testMap);
	printError(statusTest);
}

void runTests(std::string folderTestPath)
{
	std::cout << "Lancement des tests :\n" << std::endl;
	
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;
	testPlanification();
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;
	testReplanification();
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;
	testCaseInccessible();
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;
	testLectureDictionnaire();
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;
	testDeplacementSelecteur();
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;
	testRegleRepli();
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;
	
	if (std::filesystem::exists(folderTestPath) && std::filesystem::is_directory(folderTestPath))
		testValidationFichiers(folderTestPath);
	else
		std::cout << "Le chemin ne mene pas vers un dossier. Pas de test d'erreurs de fichier\n" << std::endl;
	
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;

	std::cout << "Execution des tests terminee.\n" << std::endl;
}

int main(int argc, char ** argv)
{

	switch (argc)
	{
		case 2 :
			runTests(argv[1]);
			return 0;
		case 5 :
			std::cout << "Lancement de la simulation." << std::endl;
			break;
		default :		
			std::cout << "Lancer la simulation :" << std::endl;
			std::cout << "- argument 1 : chemin de la carte" << std::endl;
			std::cout << "- argument 2 : chemin du scenario" << std::endl;
			std::cout << "- argument 3 : chemin du dossier de donnees" << std::endl;
			std::cout << "- argument 4 : chemin de la trace a ecrire" << std::endl;
			std::cout << "\nLancer les tests :" << std::endl;
			std::cout << "- argument 1 : chemin du dossier de fichiers" << std::endl;
			return 0;
	}
	
	// load with command args here instead
	Directive instructions;
	instructions.map   	 		= argv[1];
	instructions.script 		= argv[2];
	instructions.data   		= argv[3];
	std::string traceFileName	= argv[4];
	
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