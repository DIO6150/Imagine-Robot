
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
		std::cout << "Casier " << draw.location_.l << "," << draw.location_.c << std::endl;
		std::cout << "Contenu : " << draw.item_ << std::endl;
	}
}

std::string ItemStash::getItem(Pos2D pos)
{
	return drawers_[pos.c*3 + pos.l].item_;
}

bool ItemStash::removeItem(Pos2D posDrawer, std::string itemName)
{
	if(drawers_[posDrawer.c*3 + posDrawer.l].item_ == itemName)
	{
		drawers_[posDrawer.c*3 + posDrawer.l].item_ = "vide";
		return true;
	}

	return false;
}
