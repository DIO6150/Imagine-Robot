
#pragma once

#include <Robot/Errors/Error.hpp>
#include <Robot/Simulation/Script.hpp>
#include <Robot/Utils/JSONParser.hpp>

class Script;

struct ScriptParser
{
	ScriptParser(Script & script, nlohmann::json const & object);
	JSONParserStatus result;
};