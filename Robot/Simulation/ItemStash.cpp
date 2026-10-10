
#include <Robot/Simulation/ItemStash.hpp>
#include <Robot/Parsers/ClosetParser.hpp>

JSONParserStatus ItemStash::parseJSON(json const & object)
{
	return ClosetParser {*this, object}.result;
}

