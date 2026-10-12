
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

std::string ItemStash::getItem(Pos2D pos)
{
	return drawers_[pos.x*3 + pos.y].item_;
}

void ItemStash::removeItem(Pos2D posDrawer, std::string itemName)
{
	if(drawers_[posDrawer.x*3 + posDrawer.y].item_ == itemName)
		drawers_[posDrawer.x*3 + posDrawer.y].item_ = "vide";
}
