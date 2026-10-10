
#include <Robot/Simulation/Map.hpp>

JSONParserStatus Map::parseJSON(json const & object)
{
	return parser_.fillFields(object);
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

std::string Map::getName()
{
	return name_;
}

uint32_t Map::getWidth()
{
	return width_;
}

uint32_t Map::getHeight()
{
	return height_;
}
Pos2D Map::getRobotStart()
{
	return robotStart_;
}

Pos2D Map::getPosStash()
{
	return posStash_;
}

Pos2D Map::getPosDictionary()
{
	return posDictionary_;
}

std::vector<Resident> Map::getResidentList()
{
	return residentList_;
}

void Map::mapInfos()
{
	std::cout << "Nom carte : " << name_ << std::endl;
	std::cout << "Dimensions : " << width_ << " x " << height_ << std::endl;
	std::cout << "Depart Robot : " << robotStart_.x << "," << robotStart_.y << std::endl;
	std::cout << "Case armoire : " << posStash_.x << "," << posStash_.y << std::endl;
	std::cout << "Case dictionnaire : " << posDictionary_.x << "," << posDictionary_.y << std::endl;
	for (Resident res : residentList_)
		std::cout << "Resident " << res.pos_.x << "," << res.pos_.y << std::endl;
}