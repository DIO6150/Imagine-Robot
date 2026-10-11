
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
		std::cout << "Casier " << draw.location_.x << "," << draw.location_.y << std::endl;
		std::cout << "Contenu : " << draw.item_ << std::endl;
	}
}