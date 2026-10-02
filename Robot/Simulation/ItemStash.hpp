#pragma once

#include <Robot/Utils/JSONParser.hpp>

#include <Robot/Common/Item.hpp>
class ItemStash
{
public:
	JSONParserStatus parseJSON(json const & object);

	Item getItem(Pos2D pos);

private:
	std::vector<Item> items_;
};