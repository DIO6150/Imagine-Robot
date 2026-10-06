
#include <Robot/Simulation/Map.hpp>

JSONParserStatus Map::parseJson(json const & object)
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

uint32_t Map::getWidth()
{
	return width_;
}

uint32_t Map::getHeight()
{
	return height_;
}
