#include <Robot/Simulation/ItemStash.hpp>

JSONParserStatus ItemStash::parseJson(json const & object)
{
	return parser_.fillFields(object);
}