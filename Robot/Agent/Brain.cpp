
#include <Robot/Agent/Brain.hpp>

Brain::Brain(AgentControllerListener * body)
	: body_ {body}
{

}

inline int32_t calculateIndex(Pos2D pos, MapInfo info)
{
	return pos.y * info.width + pos.x;
}

inline bool inRect(Pos2D pos, uint32_t bottom, uint32_t up, uint32_t left, uint32_t right)
{
	if (pos.y < bottom || pos.y >= up || pos.x < left || pos.x >= right) return false;

	return true;
}

void Brain::simulationProgress()
{
	std::cout << "Total request : " << requestList_.size() << std::endl;
	std::cout << "Request n" << requestList_[currentReq_].getId() << std::endl;
	std::cout << "Current Objective : ";
	switch (currentObjective_)
	{
	case Objective::ReachDictionary:
		std::cout << "Reach the dictionary." << std::endl;
		break;

	case Objective::InterpretRequest:
		std::cout << "Identify nature of request." << std::endl;
		break;

	case Objective::ReachCloset:
		std::cout << "Reach the closet." << std::endl;
		break;
	
	case Objective::FindItem:
		std::cout << "Find comfort item." << std::endl;
		break;

	case Objective::TakeItem:
		std::cout << "Retrieve comfort item." << std::endl;
		break;
	
	case Objective::ReachResident:
		std::cout << "Reach the resident." << std::endl;
		break;

	case Objective::GiveItem:
		std::cout << "Give item to resident." << std::endl;
		break;

	default:
		std::cout << "No objective." << std::endl;
		break;
	}
	
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
	//currentPath_ = shortestPath(pos_, mapInfo_.dictionaryPos);
	destinationReached_ = false;
}

void Brain::tick()
{
	simulationProgress();
	std::cout << pos_.x << "," << pos_.y << std::endl;
	std::cout << "Agent perception." << std::endl;
	// PERCEVOIR
	auto result = body_->see();

	if (result != CommandStatus::Ok)
	{
		return;
	}

	std::cout << "Agent mermorization." << std::endl;
	// MEMORISER
	std::vector<Orientation> const directions = { North, West, South, East };

	for (auto i = 0; i < directions.size(); ++i)
	{
		auto tile = result.get()[i];
		
		if (inRect(tile.pos_, 0, mapInfo_.width, 0, mapInfo_.height))
			continue;
		
		auto index = calculateIndex(tile.pos_, mapInfo_);

		mentalMap_[index] = tile;
	}

	std::cout << "Agent decision." << std::endl;
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
		where = mapInfo_.residents_[requestList_[currentReq_].getIdResident()].pos_;
		break;

	case Objective::GiveItem:
		shouldGiveItem = true;
		break;
	
	default:
		break;
	}

	std::cout << "Agent action." << std::endl;
	// AGIR
	if (shouldMove)
	{
		currentPath_ = shortestPath(pos_, where);
		auto next = currentPath_.front();
		std::cout << "(" << next.x << "," << next.y << ")" << std::endl;
		auto direction = findDirection(next, pos_);

		auto result = body_->move(direction);

		pos_ += directionToPos(direction);

		for (auto dir : directions)
		{
			destinationReached_ = ((pos_ + directionToPos(dir) == where) ? true : destinationReached_);
		}

		if (destinationReached_)
		{
			if (currentObjective_ == Objective::ReachDictionary)
				currentObjective_ = Objective::InterpretRequest;
			if (currentObjective_ == Objective::ReachCloset)
			{
				currentObjective_ = Objective::FindItem;
				drawerCheckQueue_ = closetPath();
			}
			if (currentObjective_ == Objective::ReachResident)
				currentObjective_ = Objective::GiveItem;
			simulationProgress();
		}
	}
	if (shouldConsult)
	{
		auto result = body_->consult();
		if (result != CommandStatus::Ok)
		
		currentSearchedEmotion_ = std::get<0>(result.get());
		currentSearchedIntensity_ = std::get<1>(result.get());
		currentObjective_ = Objective::ReachCloset;
		//currentPath_ = shortestPath(pos_, mapInfo_.stashPos);
		destinationReached_ = false;
		simulationProgress();
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

		if (result.get() == 1) {
			currentObjective_ = Objective::TakeItem;
			simulationProgress();
			std::queue<Pos2D> empty;
    		std::swap(drawerCheckQueue_, empty);
		}
	}
	if (shouldTakeItem)
	{
		auto result = body_->take(itemName_);
		drawersContainsItem_[casier_.x*3 + casier_.y] = -1;
		carryItem_ = true;
		currentObjective_ = Objective::ReachResident;
		Pos2D residentPos = mapInfo_.residents_[requestList_[currentReq_].getIdResident()].pos_;
		//currentPath_ = shortestPath(pos_, residentPos);
		destinationReached_ = false;
		simulationProgress();
	}
	if (shouldGiveItem)
	{
		auto result = body_->give(itemName_);
		itemName_ = "vide";
		carryItem_ = false;
		currentReq_++;
		currentObjective_ = ((noRequestsLeft()) ? Objective::NoObjective : Objective::ReachDictionary);
		//if (currentObjective_ == Objective::ReachDictionary)
		//	currentPath_ = shortestPath(pos_, mapInfo_.dictionaryPos);
		destinationReached_ = false;
		simulationProgress();
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
			if (!inRect(pos, 1, mapInfo_.height - 1, 1, mapInfo_.width - 1))
			{
				mentalMap_[index].setSolid(true);
			}

			if(pos.x == 6)
			std::cout << mentalMap_[index].isSolid();
		}
		std::cout << " " << std::endl;
	}

	mentalMap_[calculateIndex(mapInfo_.robotStartPos, mapInfo_)].setAgentStart(true);
	mentalMap_[calculateIndex(mapInfo_.stashPos, mapInfo_)]     .setSolid(true).setItemPickup(true);
	mentalMap_[calculateIndex(mapInfo_.dictionaryPos, mapInfo_)].setSolid(true).setRecord(true);

	for (auto resident : mapInfo_.residents_)
	{
		mentalMap_[calculateIndex(resident.second.pos_, mapInfo_)].setSolid(true).setPerson(true);
	}
}


std::queue<Pos2D> Brain::shortestPath(Pos2D start, Pos2D destination)
{
	std::queue<Pos2D> queue;
	if(start == destination) {
		queue.push(start);
		return queue;
	}
	std::cout << "(" << start.x << "," << start.y << ") ";
	std::cout << "(" << destination.x << "," << destination.y << ") ";
	std::cout << "." << std::endl;
	
	std::queue<Pos2D> empty;
	std::map<Pos2D, Pos2D> previous;
	std::vector<Pos2D> neighbours(4);
	Pos2D currentPos;
	std::vector<Tile> surroundings;

	queue.push(start);
	previous.emplace(start, Pos2D {-1, -1});

	while(!queue.empty())
	{
		currentPos = queue.front();
		queue.pop();

		
		std::cout << "***" << currentPos.x << "," << currentPos.y << "***" << std::endl;
		for (int i = 0; i < queue.size(); i++) {
			Pos2D pos = queue.front();
			queue.pop();
			std::cout << "(" << pos.x << "," << pos.y << ") ";
			queue.push(pos);
		}
		std::cout << " -> queue." << std::endl;
		

		neighbours[0] = currentPos + directionToPos(Orientation::North);
		neighbours[1] = currentPos + directionToPos(Orientation::South);
		neighbours[2] = currentPos + directionToPos(Orientation::East);
		neighbours[3] = currentPos + directionToPos(Orientation::West);

		for(Pos2D voisin : neighbours)
		{
			
			for (auto& p : previous)
        	{
				std::cout << p.first.x << "," << p.first.y << " : ";
				std::cout << p.second.x << "," << p.second.y << ". ";
			}
			std::cout << " -> voisins." << std::endl;
			
			std::cout << currentPos.x << "," << currentPos.y << " : ";
			std::cout << voisin.x << "," << voisin.y << " -> ";
			std::cout << previous.contains(voisin) << " et " << mentalMap_[calculateIndex(voisin, mapInfo_)].isSolid() << std::endl;
			
			if(voisin == destination)
			{
				previous.emplace(voisin, currentPos);
				Pos2D pred = voisin;
				std::vector<Pos2D> toReverse;
				std::cout << "chemin trouve" << std::endl;
				while(previous.at(pred) != start)
				{
					//std::cout << pred.x << "," << pred.y << std::endl;
					pred = previous.at(pred);
					toReverse.push_back(pred);
				}

				std::swap(queue, empty);
				for(size_t i = toReverse.size()-1; i > 0; i--)
					queue.push(toReverse[i]);
				for (int i = 0; i < queue.size(); i++) {
					Pos2D pos = queue.front();
					queue.pop();
					std::cout << "(" << pos.x << "," << pos.y << ") ";
					queue.push(pos);
				}
				std::cout << "." << std::endl;
				return queue;
			}
			
			if(!previous.contains(voisin) && !(mentalMap_[calculateIndex(voisin, mapInfo_)].isSolid()))
			{
				previous.emplace(voisin, currentPos);
				queue.push(voisin);
			}
		}
	}
	std::swap(queue, empty);
	return queue;
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
