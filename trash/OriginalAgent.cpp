
#include <trash/OriginalAgent.hpp>

OriginalAgent::OriginalAgent(OriginalAgent * listener)
	: listener_ {listener}
{

}

void OriginalAgent::start(std::vector<Request> requests)
{
	// idea behind the job system
	pushJob(moveTowards(dictionary_));
	pushJob(consult());
	pushJob(moveTowards(itemStash_));
	pushJob(takeItem());
	pushJob(moveTowards(currentResident_));
	pushJob(giveItem());
	pushJob(endRequest());
}

void OriginalAgent::tick()
{
	if (frontJobs_.empty())
		return;
		
	Job & currentJob = frontJobs_.front();

	pushSafe_ = false;
	auto result = currentJob();
	pushSafe_ = true;

	if (result == Stop)
	{
		frontJobs_.pop();
	}
	else if (result == Swap)
	{
		frontJobs_.swap(backJobs_);

		std::queue<Job> empty;
		backJobs_.swap(empty);
	}
	else if (result == Break)
	{
		std::queue<Job> empty1;
		std::queue<Job> empty2;
		frontJobs_.swap(empty1);
		backJobs_.swap(empty2);
	}
}

Pos2D OriginalAgent::getPosition() const
{
	return pos_;
}

void OriginalAgent::setPosition(Pos2D newPos)
{
	pos_ = newPos;
}

void OriginalAgent::pushJob(Job const & job)
{
	if (pushSafe_)
	{
		frontJobs_.push(job);
	}
	else
	{
		backJobs_.push(job);
	}
}

Job OriginalAgent::moveTowards(Pos2D pos)
{
	return [this, pos]()
	{
		// code here

		return Stop;
	};
}

Job OriginalAgent::consult()
{
	return [this]()
	{
		auto result = listener_->consult();
		if (!result) return Break;
		// TODO : do something with the result
		return Stop;
	};
}

Job OriginalAgent::takeItem()
{
	return [this]()
	{
		auto result = listener_->take(wantedItem_);
		if (!result) return Break;

		carriedItem_ = result.get();

		return Stop;
	};
}

Job OriginalAgent::giveItem()
{
	return [this]()
	{
		if (!carriedItem_.has_value())
		{
			listener_->onAgentFail(AgentFailureType::GivingItem, "The agent didn't carry any items");
			return Break;
		}

		auto result = listener_->give(carriedItem_.value());

		if (result == CommandStatus::ResidentNotInRange)
		{
			listener_->onAgentFail(AgentFailureType::GivingItem, "The agent is not in range of a resident");
			return Break;
		}

		return Stop;
	};
}

Job OriginalAgent::endRequest()
{
	return []()
	{
		return Stop;
	};
}
uint32_t reconstructPath(std::vector<Pos2D> & chemin, std::map<Pos2D, Pos2D> predecesseur, Pos2D end)
{
	uint32_t distance = 1;
	chemin.push_back(end);
	while(predecesseur[chemin.back()].x != -1)
	{
		chemin.push_back(predecesseur[chemin.back()]);
		distance++;
	}
	return distance;
}

int32_t OriginalAgent::bfs(std::vector<Pos2D> & path, Pos2D start, Pos2D destination)
{
	std::map<Pos2D, Pos2D> predecesseur;
	std::queue<Pos2D> queue;
	std::vector<Tile> voisins(4);
	std::vector<Pos2D> chemin;
	Pos2D initPos = pos_;
	AgentTileView surroundings;
	int32_t distance = 0;

	if(start == destination) {
		chemin.push_back(start);
		return distance;
	}

	while(!queue.empty())
	{
		pos_ = queue.front();
		queue.pop();
		predecesseur[start] = Pos2D {-1, -1};

		surroundings = listener_->see();
		voisins[0] = surroundings.north_;
		voisins[1] = surroundings.east_;
		voisins[2] = surroundings.south_;
		voisins[3] = surroundings.west_;

		for(Tile voisin : voisins)
		{
			if(!predecesseur.contains(voisin.pos_) && !voisin.isSolid())
			{
				predecesseur[voisin.pos_] = pos_;
				if(voisin.pos_ == destination) {
					pos_ = initPos;
					return reconstructPath(chemin, predecesseur, voisin.pos_);
				}
			}
			queue.push(voisin.pos_);
		}
		pos_ = initPos;
		return -1;
	}
}

void OriginalAgent::initalizeMentalMap(uint32_t height, uint32_t width)
{
	std::vector<Tile> mentalArrangement(height*width);
	std::vector<int32_t> mentalTileDistances(height*width, -1);
	TileProperty wall, walkable, agentstart;
	wall.setSolid(true);
	agentstart.setAgentStart(true);
	for(uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			mentalArrangement[y*height + x] = ((x == 0 || x == width-1 || y == 0 || y == width-1) ? Tile(wall, Pos2D {x, y}) : Tile(walkable, Pos2D {x, y}));
		}
	}

	mentalArrangement[pos_.y*height + pos_.x] = Tile(agentstart, pos_);
	mentalTileDistances[pos_.y*height + pos_.x] = 0;
	
	for(uint32_t y = 1; y < height-1; ++y)
	{
		for (uint32_t x = 1; x < width-1; ++x)
		{
			mentalTileDistances[y*height + x] = bfs(pos_, Pos {x,y});
		}
	}

	mentalArrangement_ = mentalArrangement;
	mentalTileDistances_ = mentalTileDistances;
}

void OriginalAgent::start(std::vector<Request> list_requests)
{
	requests_ = list_requests;
	for(Request currentRequest : list_requests)
	{
		std::cout << "Requete numero "       << currentRequest.getId()                      << std::endl;
		std::cout << "Demande du resident "  << currentRequest.getResidentId()              << std::endl;
		std::cout << "Contenu du message : " << currentRequest.getMessage()    << std::endl << std::endl;
	}
}

void OriginalAgent::tick()
{
	
}
