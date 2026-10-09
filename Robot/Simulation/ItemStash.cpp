#include <Robot/Simulation/ItemStash.hpp>

JSONParserStatus ItemStash::parseJSON(json const & object)
{
	return parser_.fillFields(object);
}

