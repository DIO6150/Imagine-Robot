
#pragma once

#include <Robot/Errors/Error.hpp>

#include <nlohmann/json.hpp>

class Script;

struct ScriptParser
{
	ScriptParser(Script & script, nlohmann::json const & object);
	JSONParserStatus result;
};