
#include <Robot/Simulation/ItemStash.hpp>
#include <Robot/Parsers/ClosetParser.hpp>

JSONParserStatus ItemStash::parseJSON(json const & object)
{
	return ClosetParser {*this, object}.result;
}

Pos2D ItemStash::getCasierDepart()
{
	return casierDepart_;
}

void ItemStash::closetInfos()
{
	std::cout << "Nom armoire : " << name_ << std::endl;
	for (Drawer draw : drawers_)
	{
		std::cout << "Casier (" << draw.location_.l << "," << draw.location_.c;
		std::cout << "), contenu : " << draw.item_ << std::endl;
	}
}

std::optional<Item> ItemStash::getItem(Pos2D pos)
{
	auto const & drawer = drawers_[pos.l*8 + pos.c];

	if (drawer.item_ == "vide")
		return std::nullopt;

	Item result;
	result.name      = drawer.item_;
	result.emotion   = drawer.emotion_;
	result.intensity = drawer.intensity_;
	result.pos = drawer.location_;

	return result;
}

std::optional<Item> ItemStash::removeItem(Pos2D pos)
{
	auto item = getItem(pos);
	if (item) drawers_[pos.l*8 + pos.c].item_ = "vide";

	return item;
}
