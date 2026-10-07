#include <Robot/Simulation/Dictionary.hpp>

JSONParserStatus Dictionary::parseJSON(json const & object)
{
	return parser_.fillFields(object);
}

