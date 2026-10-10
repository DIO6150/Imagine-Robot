
#include <Robot/Agent/Brain.hpp>

Brain::Brain(AgentControllerListener * body)
	: body_ {body}
{

}

void Brain::start(std::vector<Request> requests)
{
	requests_ = requests;
	for(Request currentRequest : requests)
	{
		std::cout << "Requete numero :"      << currentRequest.getId()                      << std::endl;
		std::cout << "Demande du resident :" << currentRequest.getResidentId()              << std::endl;
		std::cout << "Contenu du message : " << currentRequest.getMessage()    << std::endl << std::endl;
	}
}

void Brain::tick()
{
	// PERCEVOIR
	auto nwse = body_->see();

	// MEMORISER

	// DECIDER

	// AGIR
}

uint32_t reconstructPath(std::vector<Pos2D> & chemin, std::map<Pos2D, Pos2D> predecesseur, Pos2D end)
{
	/*
	uint32_t distance = 1;
	chemin.push_back(end);
	while(predecesseur[chemin.back()].x != -1)
	{
		chemin.push_back(predecesseur[chemin.back()]);
		distance++;
	}
	return distance;
	*/

	return 0;
}

int32_t Brain::bfs(std::vector<Pos2D> & path, Pos2D start, Pos2D destination)
{
	/*
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

		surroundings = body_->see();
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
	*/

	return 0;
}

void Brain::initalizeMentalMap(uint32_t height, uint32_t width)
{
	/*
	std::vector<Tile> mentalArrangement(height*width);
	std::vector<int32_t> mentalTileDistances(height*width, -1);
	TileProperty wall, walkable, Brainstart;
	wall.setSolid(true);
	Brainstart.setAgentStart(true);
	for(uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			mentalArrangement[y*height + x] = ((x == 0 || x == width-1 || y == 0 || y == width-1) ? Tile(wall, Pos2D {x, y}) : Tile(walkable, Pos2D {x, y}));
		}
	}

	mentalArrangement[pos_.y*height + pos_.x] = Tile(Brainstart, pos_);
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
	*/
}
