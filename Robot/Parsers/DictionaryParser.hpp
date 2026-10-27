
#pragma once

#include <Robot/Errors/Error.hpp>
#include <Robot/Simulation/Dictionary.hpp>
#include <Robot/Utils/JSONParser.hpp>

class Dictionary;

struct DictionaryParser
{
	DictionaryParser(Dictionary & dictionary, nlohmann::json const & object);
	JSONParserStatus result;
};