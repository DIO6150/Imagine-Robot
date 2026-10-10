
#include <Robot/Simulation/Dictionary.hpp>
#include <Robot/Parsers/DictionaryParser.hpp>

JSONParserStatus Dictionary::parseJSON(json const & object)
{
	return DictionaryParser {*this, object}.result;
}

