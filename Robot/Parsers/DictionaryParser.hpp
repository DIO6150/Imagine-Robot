
#pragma once

#include <Robot/Errors/Error.hpp>
#include <Robot/Simulation/Dictionary.hpp>

#include <Robot/Utils/JSONParser.hpp>

#include <nlohmann/json.hpp>

#include <map>

class Dictionary;

struct DictionaryParser
{
	DictionaryParser(Dictionary & dictionary, nlohmann::json const & object);
	JSONParserStatus result;
};