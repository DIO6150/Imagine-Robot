
#include <Robot/Agent/Agent.hpp>

Agent::Agent(AgentCommandListener * listener)
	: listener_ {listener}
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

uint32_t reconstructionPath(std::vector<Pos2D> & chemin, std::map<Pos2D, Pos2D> predecesseur, Pos2D end)
{
	uint32_t distance = 1;
	chemin.push_back(end);
	while(predecesseur[chemin.back].x != -1)
	{
		chemin.push_back(predecesseur[chemin.back]);
		distance++;
	}
	return distance;
}

int32_t Agent::cheminDistance(std::vector<Pos2D> & chemin, Pos2D start, Pos2D destination)
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
			if(!predecesseur.contains(voisin.pos) && !voisin.isSolid())
			{
				predecesseur[voisin.pos] = pos_;
				if(voisin.pos == destination) {
					pos_ = initPos;
					return reconstructionPath(chemin, predecesseur, voisin.pos);
				}
			}
			queue.push(voisin.getPos());
		}
		pos_ = initPos;
		return -1;
	}
}

void Agent::initalizeMentalMap(RecapMap recap)
{
	std::vector<int32_t> mentalTileDistances(recap.height*recap*width);
	for(uint32_t y = 0; y < recap.height; ++y)
	{
		for (uint32_t x = 0; x < recap.width; ++x)
		{
			mentalArrangement[y*recap.height + x] = ((x == 0 || x == recap.width-1 || y == 0 || y == recap.width-1) ? -5 : -1);
		}
	}

	mentalTileDistances[recap.robotStart.y*height + recap.robotStart.x] = 0;
	mentalTileDistances[recap.posStash.y*height + recap.posStash.x] = -2;
	mentalTileDistances[recap.posDictionary.y*height + recap.posDictionary.x] = -3;
	for(Pos2D pres : recap.posResidentList)
		mentalTileDistances[pres.y*height + pres.x] = -4;

	mentalMap_ = mentalTileDistances;
}

void Agent::start(std::vector<Request> list_requests)
{
	requests_ = list_requests;
	for(Request currentRequest : list_requests)
	{
		std::cout << "Requete numero " << currentRequest.getId() << std::endl;
		std::cout << "Demande du resident " << currentRequest.getResidentId() << std::endl;
		std::cout << "Contenu du message : " << currentRequest.getMessage() << "\n" << std::endl;
	}

	initalizeMentalMap()
}

void Agent::tick()
{

}