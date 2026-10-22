#include <string>
#include <fstream>
#include <Robot/Simulation/Simulation.hpp>
#include <vendor/nlohmann/json.hpp>

using ordered_json = nlohmann::ordered_json;
using json = nlohmann::json;

int main(int argc, char ** argv)
{
	// load with command args here instead
	Directive instructions;
	instructions.map    = "data/cartes/appartement_01.json";
	instructions.script = "data/cartes/scenario_01.json";
	instructions.data   = "data/donnees";
	std::string traceFileName  = "trace.json";
	
	Simulation sim;
	DirectiveTrace trace;
	int statusSim = sim.executeDirective(instructions, trace);
	if (statusSim != 0)
		return statusSim;

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