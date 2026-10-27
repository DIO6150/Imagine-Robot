
#include <Robot/Simulation/Map.hpp>

#include <Robot/Parsers/MapParser.hpp>

JSONParserStatus Map::parseJSON(nlohmann::json const & object)
{
	return MapParser {*this, object}.result;
}

// TODO: what do we do when an invalid position is inputed
Tile Map::getTile(int32_t l, int32_t c) const
{
	auto index = l * width_ + c;
	return tiles_.at(index);
}

Tile Map::getTile(Pos2D pos) const
{
	return getTile(pos.l, pos.c);
}

bool Map::isInside(Pos2D pos) const
{
	if (pos.c < 0 || pos.l < 0)
		return false;

	if (pos.c >= width_ || pos.l >= height_)
		return false;

	return true;
}

std::string Map::getName() const
{
	return name_;
}

uint32_t Map::getWidth() const
{
	return width_;
}

uint32_t Map::getHeight() const
{
	return height_;
}
Pos2D Map::getRobotStartPos() const
{
	return robotStartPos_;
}

Pos2D Map::getStashPos() const
{
	return stashPos_;
}

Pos2D Map::getDictionaryPos() const
{
	return dictionaryPos_;
}

Resident Map::atResident(std::string idRes) const
{
	Resident retour;
	if(residents_.contains(idRes)) 
		retour = residents_.at(idRes);
	else
	{
		retour.name_ = "Non identifie";
		retour.pos_  = Pos2D {-1, -1};
	}
	return retour;
}

MapInfo Map::generateInfo() const
{
	MapInfo info;
	info.width  = width_;
	info.height = height_;
	info.robotStartPos = robotStartPos_;
	info.stashPos      = stashPos_;
	info.dictionaryPos = dictionaryPos_;
	info.residents_  = residents_;

	return info;
}

void Map::printInfo() const
{
	std::cout << "Nom carte : "         << name_  << std::endl;
	std::cout << "Dimensions : "        << width_ << "*" << height_ << std::endl;
	std::cout << "Depart Robot : "      << robotStartPos_.l << "," << robotStartPos_.c << std::endl;
	std::cout << "Case armoire : "      << stashPos_     .l << "," << stashPos_     .c << std::endl;
	std::cout << "Case dictionnaire : " << dictionaryPos_.l << "," << dictionaryPos_.c << std::endl;

	for (auto & res : residents_)
		std::cout << "Resident " << res.second.pos_.l << "," << res.second.pos_.c << std::endl;
}
