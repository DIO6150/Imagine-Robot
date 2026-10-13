
#include <Robot/Agent/Brain.hpp>

#include <glad/glad.h>

inline int32_t calculateIndex(Pos2D pos, MapInfo info)
{
	return pos.y * info.height + pos.x;
}

inline bool inRect(Pos2D pos, uint32_t width, uint32_t height)
{
	if (pos.x < 0 || pos.y < 0) return false;
	if (pos.x >= width || pos.y >= height) return false;

	return true;
}

Brain::Brain(AgentControllerListener * body)
	: body_ {body}
{

}

void Brain::start(MapInfo info, std::vector<Request> requestList)
{
	mapInfo_ = info;

	initalizeMentalMap();

	pos_ = mapInfo_.robotStartPos;
	currentObjective_ = Objective::FindDictionary;
}

struct DeducedStates
{
	bool perceptionChanged = false;
	bool  shouldMove = false;
	Pos2D shouldMoveWhere;
	bool shouldTakeItem = false;
	bool shouldGiveItem = false;
};

void Brain::tick()
{
	// PERCEVOIR
	DeducedStates reasoning;

	auto nwse = body_->see();

	if (!nwse)
	{
		return;
	}

	// MEMORISER
	std::vector<Orientation> const directions = { North, West, South, East };

	for (auto i = 0; i < directions.size(); ++i)
	{
		auto tile = nwse.get()[i];
		
		if (inRect(tile.pos_, mapInfo_.width, mapInfo_.height))
			continue;
		
		auto index = calculateIndex(tile.pos_, mapInfo_);

		if (mentalMap_.at(index) != tile)
		{
			reasoning.perceptionChanged = true;
		}

		mentalMap_[index] = tile;
	}

	// DECIDER

	switch (currentObjective_)
	{
	case Objective::InterpretRequest:
		break;

	case Objective::FindDictionary:
		break;
	
	case Objective::FindItem:
		break;

	case Objective::TakeItem:
		break;
	
	case Objective::FindPerson:
		break;

	case Objective::GiveItem:
		break;
	
	default:
		break;
	}

	// AGIR
	if (reasoning.shouldMove)
	{
		if (reasoning.perceptionChanged)
		{
			currentPath_ = shortestPath(pos_, reasoning.shouldMoveWhere);
		}

		if (currentPath_.empty())
		{
			// target reached or no valid path

			if (pos_ == reasoning.shouldMoveWhere)
			{
				// target reached
			}
			else
			{
				// no valid path
				body_->onAgentFail(AgentFailureType::Move, "No valid path to reach target");
			}
		}

		auto direction = findDirection(pos_, currentPath_[0]); // findDirection is maybe wrong

		auto result = move(direction);

		if (result)
		{
			currentPath_.erase(currentPath_.begin());
		}
		else
		{
			body_->onAgentFail(AgentFailureType::Move, "Cannot move towards target");
		}
	}
}

void Brain::initalizeMentalMap()
{
	mentalMap_.clear();
	mentalMap_.resize(mapInfo_.width * mapInfo_.height);

	TileProperty empty;

	Pos2D pos;
	for(pos.y = 0; pos.y < mapInfo_.height; ++pos.y)
	{
		for (pos.x = 0; pos.x < mapInfo_.width; ++pos.x)
		{
			auto index = calculateIndex(pos, mapInfo_);

			mentalMap_.at(index) = Tile {empty, pos};
			if ((pos.y == 0 || pos.y == mapInfo_.height - 1) || (pos.x == 0 || pos.x == mapInfo_.width))
			{
				mentalMap_[index].setSolid(true);
			}
		}
	}

	mentalMap_[calculateIndex(mapInfo_.robotStartPos, mapInfo_)].setAgentStart(true);
	mentalMap_[calculateIndex(mapInfo_.stashPos, mapInfo_)]     .setSolid(true).setItemPickup(true);
	mentalMap_[calculateIndex(mapInfo_.dictionaryPos, mapInfo_)].setSolid(true).setRecord(true);

	for (auto resident : mapInfo_.residentList)
	{
		mentalMap_[calculateIndex(resident.pos_, mapInfo_)].setSolid(true).setPerson(true);
	}
}

std::vector<Pos2D> reconstructPath(std::map<Pos2D, Pos2D> previous, Pos2D end)
{
	std::vector<Pos2D> path;
	path.push_back(end);

	Pos2D pred = previous.at(path.back());

	while(pred.x != -1)
	{
		path.push_back(pred);
		pred = previous.at(path.back());
	}

	std::reverse(path.begin(), path.end());

	return path;
}

std::vector<Pos2D> Brain::shortestPath(Pos2D start, Pos2D destination)
{
	std::map<Pos2D, Pos2D> previous;
	std::queue<Pos2D> queue;
	std::vector<Pos2D> neighbours(4);
	std::vector<Pos2D> path;
	Pos2D currentPos;
	std::vector<Tile> surroundings;

	if(start == destination)
	{
		path.push_back(start);
		return path;
	}

	queue.push(start);

	while(!queue.empty())
	{
		currentPos = queue.front();
		queue.pop();
		previous.emplace(start, Pos2D {-1, -1});

		neighbours[0] = currentPos + directionToPos(Orientation::North);
		neighbours[1] = currentPos + directionToPos(Orientation::South);
		neighbours[2] = currentPos + directionToPos(Orientation::East);
		neighbours[3] = currentPos + directionToPos(Orientation::West);

		for(Pos2D voisin : neighbours)
		{
			if(!previous.contains(voisin) && !(mentalMap_[calculateIndex(voisin, mapInfo_)].isSolid()))
			{
				previous.emplace(voisin, currentPos);

				if(voisin == destination)
				{
					return reconstructPath(previous, voisin);
				}
			}
			queue.push(voisin);
		}
	}

	return {};
}

bool Brain::move(Orientation direction)
{
	auto result = body_->move(direction);
	if (!result) return false;

	pos_ += directionToPos(direction);

	return true;
}