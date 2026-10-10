
#include <Robot/Agent/Agent.hpp>

Agent::Agent(AgentCommandListener * listener)
	: listener_{listener}
{
	
}

Pos2D Agent::getPosition() const
{
	return pos_;
}

void Agent::setPosition(Pos2D newPos)
{
	pos_ = newPos;
}

void Agent::MessageIdentifyLever(bool state)
{
	MessageIdentifyActive_ = state;
}

void Agent::ItemRetrivalLever(bool state)
{
	ItemRetrivalActive_ = state;
}

void Agent::ItemDeliveryLever(bool state)
{
	ItemDeliveryActive_ = state;
}

Tile Agent::atMap(std::vector<Tile> & map, int32_t y, int32_t x)
{
	return map[y*recapMap_.height_ + x];
}

std::vector<Pos2D> reconstructionPath(std::map<Pos2D, Pos2D> predecesseur, Pos2D end)
{
	std::vector<Pos2D> chemin;
	chemin.push_back(end);
	Pos2D pred = predecesseur.at(chemin.back());
	while(pred.x != -1)
	{
		chemin.push_back(pred);
		pred = predecesseur.at(chemin.back());
	}
	std::reverse(chemin.begin(), chemin.end());
	return chemin;
}

std::vector<Pos2D> Agent::shortestPath(Pos2D start, Pos2D destination)
{
	std::map<Pos2D, Pos2D> predecesseur;
	std::queue<Pos2D> queue;
	std::vector<Pos2D> voisins(4);
	std::vector<Pos2D> chemin;
	Pos2D currentPos;
	AgentTileView surroundings;

	if(start == destination) {
		chemin.push_back(start);
		return chemin;
	}

	queue.push(start);

	while(!queue.empty())
	{
		currentPos = queue.front();
		queue.pop();
		predecesseur.emplace(start, Pos2D {-1, -1});

		voisins[0] = currentPos + orientationToPos(Orientation::North);
		voisins[1] = currentPos + orientationToPos(Orientation::South);
		voisins[2] = currentPos + orientationToPos(Orientation::East);
		voisins[3] = currentPos + orientationToPos(Orientation::West);

		for(Pos2D voisin : voisins)
		{
			if(!predecesseur.contains(voisin) && !(atMap(mentalMap_, voisin.y, voisin.x)).isSolid())
			{
				predecesseur.emplace(voisin, currentPos);
				if(voisin == destination)
					return reconstructionPath(predecesseur, voisin);
			}
			queue.push(voisin);
		}
	}
	chemin.clear();	
	return chemin;
}

bool Agent::isOutWalls(int32_t y, int32_t x)
{
	return (x <= 0 || x >= recapMap_.width_-1 || y <= 0 || y >= recapMap_.width_-1);
}

void Agent::initalizeMentalMap()
{
	std::vector<Tile> mentalArrangement(recapMap_.height_*recapMap_.width_);
	TileProperty free, wall;
	wall.setSolid(true);

	for(int32_t y = 0; y < recapMap_.height_; ++y)
	{
		for (int32_t x = 0; x < recapMap_.width_; ++x)
		{
			atMap(mentalArrangement, y, x) = ((isOutWalls(x,y)) ? Tile {wall, Pos2D {x,y}} : Tile {free, Pos2D {x,y}});
		}
	}

	atMap(mentalArrangement, recapMap_.robotStart.y, recapMap_.robotStart.x).setAgentStart(true);
	atMap(mentalArrangement, recapMap_.posStash.y, recapMap_.posStash.x).setSolid(true).setItemPickup(true);
	atMap(mentalArrangement, recapMap_.posDictionary.y, recapMap_.posDictionary.x).setSolid(true).setRecord(true);
	for(Resident pres : recapMap_.posResidentList)
		atMap(mentalArrangement, pres.pos_.y, pres.pos_.x).setSolid(true).setPerson(true);

	mentalMap_ = mentalArrangement;
}

void Agent::recapMapSet(RecapMap recap)
{
	recapMap_.width_ = recap.width_;
	recapMap_.height_ = recap.height_;
	recapMap_.robotStart = recap.robotStart;
	recapMap_.posStash = recap.posStash;
	recapMap_.posDictionary = recap.posDictionary;
	recapMap_.posResidentList = recap.posResidentList;
}

void Agent::start(std::vector<Request> list_requests, RecapMap recap)
{
	recapMapSet(recap);

	initalizeMentalMap();
	requests_ = list_requests;
	for(Request currentRequest : list_requests)
	{
		std::cout << "Requete numero " << currentRequest.getId() << std::endl;
		std::cout << "Demande du resident " << currentRequest.getResident().id_ << std::endl;
		std::cout << "Contenu du message : " << currentRequest.getMessage() << "\n" << std::endl;
	}
}

/*
CommandStatus<void> Agent::move(Orientation direction)
{
	Pos2D newPos = pos_ + orientationToPos(direction);

	if (isOutWalls(newPos.x, newPos.y))
	{
		// TODO: push to trace (view)
		return CommandStatus::AgentMovementObstructed;
	}

	auto tile = atMap(mentalMap_, newPos.x, newPos.y);
	if (tile.isSolid())
	{
		// TODO: push to trace (view)
		return CommandStatus::AgentMovementObstructed;
	}

	// TODO: push to trace (view)

	return CommandStatus::Ok;
}
*/

void Agent::tick()
{

}