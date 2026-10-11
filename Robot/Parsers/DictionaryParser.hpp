
#pragma once

#include <Robot/Errors/Error.hpp>

#include <nlohmann/json.hpp>

class Dictionary;

struct DictionaryParser
{
	DictionaryParser(Dictionary & dictionary, nlohmann::json const & object);
	JSONParserStatus result;
};