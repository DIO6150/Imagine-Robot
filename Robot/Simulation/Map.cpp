
#include <Robot/Simulation/Map.hpp>

#include <Robot/Parsers/MapParser.hpp>

JSONParserStatus Map::parseJSON(nlohmann::json const & object)
{
	return MapParser {*this, object}.result;
}

// TODO: what do we do when an invalid position is inputed
Tile Map::getTile(int32_t x, int32_t y) const
{
	auto index = x * width_ + y;
	return tiles_.at(index);
}

Tile Map::getTile(Pos2D pos) const
{
	return getTile(pos.x, pos.y);
}

bool Map::isInside(Pos2D pos) const
{
	if (pos.x < 0 || pos.y < 0)
		return false;

	if (pos.x >= width_ || pos.y >= height_)
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

std::vector<Resident> Map::getResidentList() const
{
	return residentList_;
}

MapInfo Map::generateInfo() const
{
	MapInfo info;
	info.width  = width_;
	info.height = height_;
	info.robotStartPos = robotStartPos_;
	info.stashPos      = stashPos_;
	info.dictionaryPos = dictionaryPos_;
	info.residentList  = residentList_;

	return info;
}

void Map::printInfo() const
{
	std::cout << "Nom carte : "         << name_  << std::endl;
	std::cout << "Dimensions : "        << width_ << "*" << height_ << std::endl;
	std::cout << "Depart Robot : "      << robotStartPos_.x << "," << robotStartPos_.y << std::endl;
	std::cout << "Case armoire : "      << stashPos_     .x << "," << stashPos_     .y << std::endl;
	std::cout << "Case dictionnaire : " << dictionaryPos_.x << "," << dictionaryPos_.y << std::endl;

	for (Resident res : residentList_)
		std::cout << "Resident " << res.pos_.x << "," << res.pos_.y << std::endl;
}