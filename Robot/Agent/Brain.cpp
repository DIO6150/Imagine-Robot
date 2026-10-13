
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

struct DeducedStates
{
	bool perceptionChanged = false;
	bool  shouldMove = false;
	Pos2D shouldMoveWhere;
	bool shouldTakeItem = false;
	bool shouldGiveItem = false;
	bool shouldConsult  = false;
	bool shouldSearch   = false;
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
	case Objective::ReachDictionary:
		reasoning.shouldMove      = true;
		reasoning.shouldMoveWhere = mapInfo_.dictionaryPos;
		break;

	case Objective::InterpretRequest:
		reasoning.shouldConsult = true;
		break;

	case Objective::ReachCloset:
		reasoning.shouldMove = true;
		reasoning.shouldMoveWhere = mapInfo_.stashPos;
		break;
	
	case Objective::FindItem:
		reasoning.shouldSearch = true;
		break;

	case Objective::TakeItem:
		reasoning.shouldTakeItem = true;
		break;
	
	case Objective::ReachResident:
		reasoning.shouldMove = true;
		break;

	case Objective::GiveItem:
		reasoning.shouldGiveItem = true;
		break;
	
	default:
		break;
	}

	// AGIR
	if (reasoning.shouldMove)
	{
		if (reasoning.perceptionChanged || currentPath_.empty())
		{
			currentPath_ = shortestPath(pos_, reasoning.shouldMoveWhere);
		}

		if (currentPath_.empty())
		{
			// target reached or no valid path

			if (pos_ == reasoning.shouldMoveWhere)
			{
				// target reached
				if (currentObjective_ == Objective::ReachDictionary)
					currentObjective_ = Objective::InterpretRequest;
				if (currentObjective_ == Objective::ReachCloset)
					currentObjective_ = Objective::FindItem;
				if (currentObjective_ == Objective::ReachResident)
					currentObjective_ = Objective::GiveItem;
			}
			else
			{
				// no valid path
				body_->onAgentFail(AgentFailureType::Move, "No valid path to reach target");
			}
		}
		else
		{
			auto direction = findDirection(pos_, currentPath_[0]); // findDirection is maybe wrong
	
			auto result = body_->move(direction);
	
			if (result)
			{
				pos_ += directionToPos(direction); // ajust mental position
				currentPath_.erase(currentPath_.begin());
			}
			else
			{
				body_->onAgentFail(AgentFailureType::Move, "Cannot move towards target");
			}
		}
	}

	if (reasoning.shouldConsult)
	{
		auto result = body_->consult();
		if (result != CommandStatus::Ok)
		
		currentSearchedEmotion_ = std::get<0>(result.get());
		currentSearchedIntensity_ = std::get<1>(result.get());
		currentObjective_ = Objective::ReachCloset;
		drawerCheckQueue_ = closetPath();
	}

	if (reasoning.shouldSearch)
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

	if (reasoning.shouldTakeItem)
	{
		auto result = body_->take(itemName_);
		drawersContainsItem_[casier_.x*3 + casier_.y] = -1;
		carryItem_ = true;
		currentObjective_ = Objective::ReachResident;
	}

	if (reasoning.shouldGiveItem)
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
					return reconstructPath(previous, voisin);
			}
			queue.push(voisin);
		}
	}

	return {};
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
