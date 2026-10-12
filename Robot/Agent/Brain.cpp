
#include <Robot/Agent/Brain.hpp>

Brain::Brain(AgentControllerListener * body)
	: body_ {body}
{

}


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

std::string Brain::getCurrentMessage()
{
	return requestList_[currentReq_].getMessage();
}

bool Brain::noRequestsLeft()
{
	return currentReq_ == requestList_.size();
}

void Brain::start(std::vector<Request> requestList, MapInfo info, Pos2D casierDepart)
{
	mapInfo_ = info;

	initalizeMentalMap();

	pos_ = mapInfo_.robotStartPos;
	casier_ = casierDepart;
	requestList_ = requestList;
	currentReq_ = 0;
	currentObjective_ = Objective::ReachDictionary;
}

void Brain::tick()
{
	// PERCEVOIR
	auto result = body_->see();

	if (result != CommandStatus::Ok)
	{
		return;
	}

	// MEMORISER
	std::vector<Orientation> const directions = { North, West, South, East };

	for (auto i = 0; i < directions.size(); ++i)
	{
		auto tile = result.get()[i];
		
		if (inRect(tile.pos_, mapInfo_.width, mapInfo_.height))
			continue;
		
		auto index = calculateIndex(tile.pos_, mapInfo_);

		mentalMap_[index] = tile;
	}

	// DECIDER
	bool shouldMove       = false;
	bool shouldConsult    = false;
	bool shouldSearch     = false;
	bool shouldTakeItem   = false;
	bool shouldGiveItem   = false;
	Pos2D where;
	// idk if its the right design choice
	// could have an empty std::function here instead
	// we fill it below then execute it in AGIR

	switch (currentObjective_)
	{
	case Objective::ReachDictionary:
		shouldMove = true;
		where = mapInfo_.dictionaryPos;
		break;

	case Objective::InterpretRequest:
		shouldConsult = true;
		break;

	case Objective::ReachCloset:
		shouldMove = true;
		where = mapInfo_.stashPos;
		break;
	
	case Objective::FindItem:
		shouldSearch = true;
		break;

	case Objective::TakeItem:
		shouldTakeItem = true;
		break;
	
	case Objective::ReachResident:
		shouldMove = true;
		break;

	case Objective::GiveItem:
		shouldGiveItem = true;
		break;
	
	default:
		break;
	}

	// AGIR
	if (shouldMove)
	{
		auto path      = shortestPath (pos_, where);
		auto direction = findDirection(pos_, path[0]);

		auto result = body_->move(direction);

		pos_ += directionToPos(direction);

		bool hasReachedDest = false;

		for (auto dir : directions)
		{
			hasReachedDest = ((pos_ + directionToPos(dir) == where) ? true : hasReachedDest);
		}

		if (hasReachedDest)
		{
			if (currentObjective_ == Objective::ReachDictionary)
				currentObjective_ = Objective::InterpretRequest;
			if (currentObjective_ == Objective::ReachCloset)
				currentObjective_ = Objective::FindItem;
			if (currentObjective_ == Objective::ReachResident)
				currentObjective_ = Objective::GiveItem;
		}
	}
	if (shouldConsult)
	{
		auto result = body_->consult();
		if (result != CommandStatus::Ok)
		
		currentSearchedEmotion_ = std::get<0>(result.get());
		currentSearchedIntensity_ = std::get<1>(result.get());
		currentObjective_ = Objective::ReachCloset;
		drawerCheckQueue_ = closetPath();
	}
	if (shouldSearch)
	{
		auto drawerDestination = drawerCheckQueue_.front();
		while(drawersContainsItem_[drawerDestination.x*3 + drawerDestination.y] == -1)
		{
			drawerCheckQueue_.pop();
			drawerDestination = drawerCheckQueue_.front();
		}

		Orientation dirDrawer;
		if(drawerDestination.y != casier_.y)
		{
			dirDrawer = findDirection(drawerDestination, casier_);
		}
		else
		{
			if((drawerDestination.x - casier_.x)%8 < 4)
				dirDrawer = Orientation::East;
			else
				dirDrawer = Orientation::West;
		}
		
		CommandResult<int> result = body_->search(dirDrawer);
		casier_ = casier_ + directionToPos(dirDrawer);
		casier_.x = casier_.x%8;
		drawersContainsItem_[casier_.x*3 + casier_.y] = result.get();
		if(casier_ == drawerDestination)
			drawerCheckQueue_.pop();

		if (result.get() == 1)
			currentObjective_ = Objective::TakeItem;
	}
	if (shouldTakeItem)
	{
		auto result = body_->take(itemName_);
		drawersContainsItem_[casier_.x*3 + casier_.y] = -1;
		carryItem_ = true;
		currentObjective_ = Objective::ReachResident;
	}
	if (shouldGiveItem)
	{
		auto result = body_->give(itemName_);
		itemName_ = "vide";
		carryItem_ = false;
		currentReq_++;
		currentObjective_ = ((noRequestsLeft()) ? Objective::NoObjective : Objective::ReachDictionary);
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

	for (auto resident : mapInfo_.residents_)
	{
		mentalMap_[calculateIndex(resident.second.pos_, mapInfo_)].setSolid(true).setPerson(true);
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

	if(start == destination) {
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
					return reconstructPath(previous, voisin);
			}
			queue.push(voisin);
		}
	}

	path.clear(); // clear ???

	return path;
}

std::queue<Pos2D> Brain::closetPath()
{
	int line = intensityToInt(currentSearchedIntensity_), column = emotionToInt(currentSearchedEmotion_);
	int emo_vois1d = (column + 1)%8, emo_vois1g = (column - 1)%8, emo_vois2d = (column + 2)%8, emo_vois2g = (column - 2)%8;
	std::vector<int> intensityOrder(3, 0);
	std::queue<Pos2D> queueDrawer;
	switch(line)
	{
		case 0:
			intensityOrder[1] = 1;
			intensityOrder[2] = 2;
		case 1:
			intensityOrder[0] = 1;
			intensityOrder[2] = 2;
		case 2:
			intensityOrder[0] = 2;
			intensityOrder[1] = 1;
		default:
			break;
	}
	queueDrawer.push(Pos2D{column, intensityOrder[0]});
	queueDrawer.push(Pos2D{column, intensityOrder[1]});
	queueDrawer.push(Pos2D{column, intensityOrder[2]});
	queueDrawer.push(Pos2D{emo_vois1d, intensityOrder[0]});
	queueDrawer.push(Pos2D{emo_vois1g, intensityOrder[0]});
	queueDrawer.push(Pos2D{emo_vois1d, intensityOrder[1]});
	queueDrawer.push(Pos2D{emo_vois1g, intensityOrder[1]});
	queueDrawer.push(Pos2D{emo_vois1d, intensityOrder[2]});
	queueDrawer.push(Pos2D{emo_vois1g, intensityOrder[2]});
	queueDrawer.push(Pos2D{emo_vois2d, intensityOrder[0]});
	queueDrawer.push(Pos2D{emo_vois2g, intensityOrder[0]});
	queueDrawer.push(Pos2D{emo_vois2d, intensityOrder[1]});
	queueDrawer.push(Pos2D{emo_vois2g, intensityOrder[1]});
	queueDrawer.push(Pos2D{emo_vois2d, intensityOrder[2]});
	queueDrawer.push(Pos2D{emo_vois2g, intensityOrder[2]});
	return queueDrawer;
}
