#pragma once

#include <string>
#include <vector>
#include <map>

#include <Robot/Common/TraceElement.hpp>

struct Directive
{
	std::string map;
	std::string script;
	std::string data;
	std::string trace;
};

struct DirectiveTrace
{
	std::string format = "robot-reconfort/trace";
	uint32_t version = 1;
	std::string carte;
	std::string scenario;
	std::vector<std::string> equipe = {"EOUZAN Mael", "DOMBROWSKI Tahina"};
	
	std::vector<Pas> pas;
	std::vector<Livraison> livraisons;
	std::map<std::string, uint32_t> resume;
};

