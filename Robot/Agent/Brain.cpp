
#include <Robot/Agent/Brain.hpp>

Brain::Brain(AgentControllerListener * body)
	: body_ {body}
{

}

inline int32_t calculateIndex(Pos2D pos, uint32_t width)
{
	return pos.l * width + pos.c;
}

inline bool inRect(Pos2D pos, uint32_t down, uint32_t up, uint32_t left, uint32_t right)
{
	if (pos.l < down || pos.l >= up || pos.c < left || pos.c >= right) return false;

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
		std::cout << "Current position : (" << pos_.l << "," << pos_.c << ")." << std::endl;
		break;

	case Objective::InterpretRequest:
		std::cout << "Identify nature of request." << std::endl;
		break;

	case Objective::ReachCloset:
		std::cout << "Reach the closet." << std::endl;
		std::cout << "Current position : (" << pos_.l << "," << pos_.c << ")." << std::endl;
		std::cout << "Emotion : " << emotionToInt(currentSearchedEmotion_);
		std::cout << ", Intensity : " << intensityToInt(currentSearchedIntensity_) << std::endl;
		break;
	
	case Objective::FindItem:
		std::cout << "Find comfort item." << std::endl;
		std::cout << "Current drawer : (" << casier_.l << "," << casier_.c << ")." << std::endl; 
		break;

	case Objective::TakeItem:
		std::cout << "Retrieve comfort item." << std::endl;
		break;
	
	case Objective::ReachResident:
		std::cout << "Reach the resident." << std::endl;
		std::cout << "Item retrieved : " << itemName_ << std::endl;
		std::cout << "Current position : (" << pos_.l << "," << pos_.c << ")." << std::endl;
		break;

	case Objective::GiveItem:
		std::cout << "Give item to resident." << std::endl;
		std::cout << "Item given : " << itemName_ << std::endl;
		break;

	default:
		std::cout << "No objective." << std::endl;
		break;
	}
	
}

void Brain::addStep(Orientation dir)
{
	Pas curStep;
	curStep.t = nSteps_;
	curStep.demande = currentReq_ + 1;
	curStep.position = pos_;
	curStep.casier = casier_;
	curStep.argument = "null";
	switch (currentObjective_)
	{
		case Objective::InterpretRequest:
			curStep.action = "CONSULTER";
			break;
		
		case Objective::FindItem:
			curStep.action = "CHERCHER";
			break;

		case Objective::TakeItem:
			curStep.action = "PRENDRE";
			curStep.argument = itemName_;
			break;

		case Objective::GiveItem:
			curStep.action = "DONNER";
			curStep.argument = itemName_;
			break;

		default:
			curStep.action = "AVANCER";
			break;
	}
	curStep.argument = ((currentObjective_ != Objective::InterpretRequest && currentObjective_ != Objective::TakeItem && currentObjective_ != Objective::GiveItem) ? directionToArg(dir) : curStep.argument);

	for (auto& curPer : currentPerception_)
	{
		std::string contenuCase;
		if (curPer.second.isPerson()) 			contenuCase = "resident";
		else if (curPer.second.isItemPickup())  contenuCase = "armoire";
		else if (curPer.second.isRecord()) 		contenuCase = "dictionnaire";
		else 									contenuCase = ((curPer.second.isSolid()) ? "mur" : "libre");
		curStep.perception[directionToArg(curPer.first)] = contenuCase;
	}

	curStep.contenu_casier = ((currentObjective_ == Objective::FindItem || currentObjective_ == Objective::TakeItem) ? drawersContainsItem_[casier_.l*8 + casier_.c] : "null");
    curStep.commentaire = "null";
	steps_.push_back(curStep);
}

std::string Brain::determineRepli(Pos2D casierChoisi, Emotion emoCherchee, Intensity intensCherchee)
{
	std::string repli;
	
	if (casierChoisi.c == emotionToInt(emoCherchee)) {
		if (casierChoisi.l == intensityToInt(intensCherchee))
			repli = "aucun";
		else
			repli = "intensite";
	}
	else if ((casierChoisi.c + 1)%8 == emotionToInt(emoCherchee) || (casierChoisi.c - 1)%8 == emotionToInt(emoCherchee))
		repli = "voisine_1";
	else if ((casierChoisi.c + 2)%8 == emotionToInt(emoCherchee) || (casierChoisi.c - 2)%8 == emotionToInt(emoCherchee))
		repli = "voisine_2";
	else
		repli = "casiers_vides";

	return repli;
}

void Brain::addDelivery(std::string motif)
{
	Livraison curDelivery;
	curDelivery.demande = requestList_[currentReq_].getId();
	curDelivery.resident = requestList_[currentReq_].getIdResident();

	int iEmotion = emotionToInt(currentSearchedEmotion_);
	curDelivery.emotion = ((iEmotion == -1) ? "null" : emotionToString[iEmotion]);
	int iIntensity = intensityToInt(currentSearchedIntensity_);
	curDelivery.intensity = ((iIntensity == -1) ? "null" : intensityToString[iIntensity]);

	curDelivery.casier_choisi = casier_;
	curDelivery.repli = determineRepli(casier_, currentSearchedEmotion_, currentSearchedIntensity_);

    curDelivery.objet = itemName_;
    curDelivery.pas_utilises = nStepsRequest_;
    curDelivery.succes = deliverySuccess_;
    curDelivery.motif_echec = motif;
	deliveries_.push_back(curDelivery);
}

std::vector<Pas> Brain::getSteps()
{
	return steps_;
}

std::vector<Livraison> Brain::getDeliveries()
{
	return deliveries_;
}

std::string Brain::getCurrentMessage()
{
	return requestList_[currentReq_].getMessage();
}

bool Brain::noRequestsLeft()
{
	return currentReq_ == requestList_.size();
}

void Brain::initalizeMentalMap()
{
	mentalMap_.clear();
	mentalMap_.resize(mapInfo_.width * mapInfo_.height);

	TileProperty empty;

	Pos2D pos;
	for(pos.l = 0; pos.l < mapInfo_.height; ++pos.l)
	{
		for (pos.c = 0; pos.c < mapInfo_.width; ++pos.c)
		{
			auto index = calculateIndex(pos, mapInfo_.width);

			mentalMap_.at(index) = Tile {empty, pos};
			if (!inRect(pos, 1, mapInfo_.height - 1, 1, mapInfo_.width - 1))
			{
				mentalMap_[index].setSolid(true);
			}
		}
	}

	mentalMap_[calculateIndex(mapInfo_.robotStartPos, mapInfo_.width)].setAgentStart(true);
	mentalMap_[calculateIndex(mapInfo_.stashPos, mapInfo_.width)]     .setSolid(true).setItemPickup(true);
	mentalMap_[calculateIndex(mapInfo_.dictionaryPos, mapInfo_.width)].setSolid(true).setRecord(true);

	for (auto resident : mapInfo_.residents_)
	{
		mentalMap_[calculateIndex(resident.second.pos_, mapInfo_.width)].setSolid(true).setPerson(true);
	}
}

void Brain::nextRequest(std::string motif)
{

	deliverySuccess_ = ((motif == "null") ? true : false);
	addDelivery(motif);
	currentReq_++;
	currentObjective_ = ((noRequestsLeft()) ? Objective::NoObjective : Objective::ReachDictionary);
	deliverySuccess_ = false;
	destinationReached_ = false;
	nStepsRequest_ = 0;
}

void Brain::start(std::vector<Request> requestList, MapInfo info, Pos2D casierDepart)
{
	mapInfo_ = info;

	initalizeMentalMap();

	pos_ = mapInfo_.robotStartPos;
	casier_ = casierDepart;
	requestList_ = requestList;
	currentReq_ = 0;
	nSteps_ = 0;
	nStepsRequest_ = 0;
	currentPerception_[Orientation::North] = Tile();
	currentPerception_[Orientation::West] = Tile();
	currentPerception_[Orientation::South] = Tile();
	currentPerception_[Orientation::East] = Tile();
	deliverySuccess_ = false;
	currentSearchedEmotion_ = Emotion::Unidentified;
	currentSearchedIntensity_ = Intensity::Unidentified;
	currentObjective_ = Objective::ReachDictionary;
}

struct DeducedStates
{
	bool needReplan = false;
	bool  shouldMove = false;
	Pos2D shouldMoveWhere;
	bool shouldTakeItem = false;
	bool shouldGiveItem = false;
	bool shouldConsult  = false;
	bool shouldSearch   = false;
};

void Brain::tick()
{
	simulationProgress();
	//std::cout << pos_.l << "," << pos_.c << std::endl;
	// PERCEVOIR
	DeducedStates reasoning;
	std::vector<Orientation> const directions = { North, West, South, East };
 
	//std::cout << isVisited_[pos_.l * mapInfo_.width + pos_.c] << std::endl;
	auto nwse = body_->see();

	if (!nwse)
	{
		return;
	}

	// MEMORISER
	
	for (auto i = 0; i < directions.size(); ++i)
	{
		auto tile = nwse.get()[i];
		
		if (!inRect(tile.pos_, 0, mapInfo_.height, 0, mapInfo_.width))
			continue;

		mentalMap_[calculateIndex(tile.pos_, mapInfo_.width)] = tile;
		currentPerception_[directions[i]] = tile;
		//std::cout << tile.pos_.l << "," << tile.pos_.c << " : " << mentalMap_[calculateIndex(tile.pos_, mapInfo_.width)].isSolid() << std::endl;
	}

	if (!currentPath_.empty())
	{
		Pos2D curStep;
		for(int istep = 0; istep < currentPath_.size(); istep++)
		{
			curStep = currentPath_.front();
			currentPath_.pop();
			//std::cout << curStep.l << "," << curStep.c << " : " << mentalMap_[calculateIndex(curStep, mapInfo_.width)].isSolid() << std::endl;
			reasoning.needReplan = ((mentalMap_[calculateIndex(curStep, mapInfo_.width)].isSolid()) ? true : reasoning.needReplan);
			currentPath_.push(curStep);
		}
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
		reasoning.shouldMove      = true;
		reasoning.shouldMoveWhere = mapInfo_.residents_[requestList_[currentReq_].getIdResident()].pos_;
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
		//std::cout << reasoning.needReplan << std::endl;
		if (reasoning.needReplan || currentPath_.empty())
		{
			currentPath_ = Brain::shortestPath(pos_, reasoning.shouldMoveWhere, mentalMap_, mapInfo_.width);
		}

		if (!currentPath_.empty())
		{
			auto direction = findDirection(currentPath_.front(), pos_); // findDirection is maybe wrong
			
			addStep(direction);
	
			auto result = body_->move(direction);
	
			if (result)
			{
				pos_ += directionToPos(direction); // ajust mental position
				currentPath_.pop();
			}
			else
			{
				body_->onAgentFail(AgentFailureType::Move, "Cannot move towards target");
			}
		}

		if (currentPath_.empty())
		{
			// target reached or no valid path
			bool reached = false;
			for (auto dir : directions)
			{
				reached = ((pos_ + directionToPos(dir) == reasoning.shouldMoveWhere) ? true : reached);
			}

			if (reached)
			{
				// target reached
				if (currentObjective_ == Objective::ReachDictionary)
					currentObjective_ = Objective::InterpretRequest;
				if (currentObjective_ == Objective::ReachCloset)
				{
					currentObjective_ = Objective::FindItem;
					drawerCheckQueue_ = closetPath();
				}
				if (currentObjective_ == Objective::ReachResident)
					currentObjective_ = Objective::GiveItem;
			}
			else
			{
				// no valid path
				body_->onAgentFail(AgentFailureType::Move, "La destination requise n'est pas accessible. Passage à la requete suivante");
				if (currentObjective_ == Objective::ReachDictionary)
					nextRequest("dictionnaire inaccessible");
				if (currentObjective_ == Objective::ReachCloset)
					nextRequest("armoire inaccessible");
				if (currentObjective_ == Objective::ReachResident)
					nextRequest("resident inaccessible");
			}
		}
	}

	if (reasoning.shouldConsult)
	{
		addStep(Orientation::North);
		
		auto result = body_->consult();
		//if (result != CommandStatus::Ok)
		
		currentSearchedEmotion_ = std::get<0>(result.get());
		currentSearchedIntensity_ = std::get<1>(result.get());
		if (currentSearchedEmotion_ == Emotion::Unidentified || currentSearchedIntensity_ == Intensity::Unidentified)
		{
			body_->onAgentFail(AgentFailureType::Consult, "Pas d'emotion reconnue dans le message. Passage à la requete suivante.");
			nextRequest("emotion indeterminee");
		}
		else
		{
			currentObjective_ = Objective::ReachCloset;
			destinationReached_ = false;
		}
		simulationProgress();
	}

	if (reasoning.shouldSearch)
	{
		auto drawerDestination = drawerCheckQueue_.front();
		while(drawersContainsItem_[drawerDestination.l*8 + drawerDestination.c] == "vide")
		{
			drawerCheckQueue_.pop();
			drawerDestination = drawerCheckQueue_.front();
		}

		if (drawerCheckQueue_.empty())
		{
			body_->onAgentFail(AgentFailureType::Search, "L'armoire ne contient pas d'objet adapte. Passage à la requete suivante.");
			nextRequest("armoire vide");
		}
		else{

			Orientation dirDrawer;
			//std::cout << "des : " << drawerDestination.l << ", cas : " << casier_.l << std::endl;
			if(drawerDestination.l != casier_.l)
			{
				//std::cout << "Nord/Sud" << std::endl;
				dirDrawer = findDirection(drawerDestination, casier_);
			}
			else
			{
				//std::cout << "Est/Ouest" << std::endl;
				if((drawerDestination.c - casier_.c + 8)%8 < 4)
					dirDrawer = Orientation::East;
				else
					dirDrawer = Orientation::West;
			}

			addStep(dirDrawer);

			casier_ = casier_ + directionToPos(dirDrawer);
			casier_.c = (casier_.c + 8)%8;
			CommandResult<std::string> result = body_->search(dirDrawer);
			//std::cout << "(" << casier_.l << "," << casier_.c << "), content : " << result.get() << std::endl;
			drawersContainsItem_[casier_.l*8 + casier_.c] = result.get();

			if (casier_ == drawerDestination) {
				//std::cout << "(" << drawerDestination.l << "," << drawerDestination.c << ") : " << result.get() << std::endl;
				if (result.get() != "vide")
				{
					currentObjective_ = Objective::TakeItem;
					simulationProgress();
					std::queue<Pos2D> empty;
					std::swap(drawerCheckQueue_, empty);
				}
				else
					drawerCheckQueue_.pop();
			}
		}
	}

	if (reasoning.shouldTakeItem)
	{
		addStep(Orientation::North);

		itemName_ = drawersContainsItem_[casier_.l*8 + casier_.c];
		auto result = body_->take(itemName_);
		carryItem_ = true;
		drawersContainsItem_[casier_.l*8 + casier_.c] = "vide";
		std::cout << "item taken : " << itemName_ << std::endl;
		currentObjective_ = Objective::ReachResident;
		Pos2D residentPos = mapInfo_.residents_[requestList_[currentReq_].getIdResident()].pos_;
		destinationReached_ = false;
		simulationProgress();
	}

	nStepsRequest_++;

	if (reasoning.shouldGiveItem)
	{
		addStep(Orientation::North);

		auto result = body_->give(itemName_);
		nextRequest("null");
		itemName_ = "vide";
		carryItem_ = false;
		simulationProgress();
	}

	nSteps_++;
}

std::queue<Pos2D> Brain::shortestPath(Pos2D start, Pos2D destination, std::vector<Tile> givenMap, uint32_t width)
{
	std::queue<Pos2D> queue;
	if(start == destination) {
		queue.push(start);
		return queue;
	}

	/*
	std::cout << "(" << start.l << "," << start.c << ") ";
	std::cout << "(" << destination.l << "," << destination.c << ") ";
	std::cout << "." << std::endl;
	*/
	
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

		/*
		std::cout << "***" << currentPos.l << "," << currentPos.c << "***" << std::endl;
		for (int i = 0; i < queue.size(); i++) {
			Pos2D pos = queue.front();
			queue.pop();
			std::cout << "(" << pos.l << "," << pos.c << ") ";
			queue.push(pos);
		}
		std::cout << " -> queue." << std::endl;
		*/

		neighbours[0] = currentPos + directionToPos(Orientation::North);
		neighbours[1] = currentPos + directionToPos(Orientation::South);
		neighbours[2] = currentPos + directionToPos(Orientation::East);
		neighbours[3] = currentPos + directionToPos(Orientation::West);

		for(Pos2D voisin : neighbours)
		{
			/*
			for (auto& p : previous)
        	{
				std::cout << p.first.l << "," << p.first.c << " : ";
				std::cout << p.second.l << "," << p.second.c << ". ";
			}
			std::cout << " -> voisins." << std::endl;
			
			std::cout << currentPos.l << "," << currentPos.c << " : ";
			std::cout << voisin.l << "," << voisin.c << " -> ";
			std::cout << previous.contains(voisin) << " et " << mentalMap_[calculateIndex(voisin, mapInfo_.width)].isSolid() << std::endl;
			*/

			if(voisin == destination)
			{
				std::vector<Pos2D> toReverse;
				//std::cout << "chemin trouve : ";
				while(currentPos != start)
				{
					//std::cout << currentPos.l << "," << currentPos.c << std::endl;
					toReverse.push_back(currentPos);
					currentPos = previous.at(currentPos);
				}

				std::swap(queue, empty);
				for(int i = toReverse.size()-1; i >= 0; i--)
					queue.push(toReverse[i]);
				
				/*
				for (int i = 0; i < queue.size(); i++) {
					Pos2D pos = queue.front();
					queue.pop();
					std::cout << "(" << pos.l << "," << pos.c << ") ";
					queue.push(pos);
				}
				std::cout << "." << std::endl;
				*/
				return queue;
			}
			
			if(!previous.contains(voisin) && !(givenMap[calculateIndex(voisin, width)].isSolid()))
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
	int emo_vois1d = (column + 1)%8, emo_vois1g = (column - 1 + 8)%8, emo_vois2d = (column + 2)%8, emo_vois2g = (column - 2 + 8)%8;
	std::vector<int> intensityOrder(2);
	std::queue<Pos2D> queueDrawer;
	switch(line)
	{
		case 0:
			intensityOrder[0] = 1;
			intensityOrder[1] = 2;
			break;
		case 1:
			intensityOrder[0] = 0;
			intensityOrder[1] = 2;
			break;
		case 2:
			intensityOrder[0] = 1;
			intensityOrder[1] = 0;
			break;
		default:
			std::cout << "invalide :" << line << std::endl;
			break;
	}
	queueDrawer.push(Pos2D{line, column});
	queueDrawer.push(Pos2D{intensityOrder[0], column});
	queueDrawer.push(Pos2D{intensityOrder[1], column});
	queueDrawer.push(Pos2D{line, emo_vois1d});
	queueDrawer.push(Pos2D{line, emo_vois1g});
	queueDrawer.push(Pos2D{intensityOrder[0], emo_vois1d});
	queueDrawer.push(Pos2D{intensityOrder[0], emo_vois1g});
	queueDrawer.push(Pos2D{intensityOrder[1], emo_vois1d});
	queueDrawer.push(Pos2D{intensityOrder[1], emo_vois1g});
	queueDrawer.push(Pos2D{line, emo_vois2d});
	queueDrawer.push(Pos2D{line, emo_vois2g});
	queueDrawer.push(Pos2D{intensityOrder[0], emo_vois2d});
	queueDrawer.push(Pos2D{intensityOrder[0], emo_vois2g});
	queueDrawer.push(Pos2D{intensityOrder[1], emo_vois2d});
	queueDrawer.push(Pos2D{intensityOrder[1], emo_vois2g});

	for (int i = 0; i < queueDrawer.size(); i++) {
		Pos2D pos = queueDrawer.front();
		queueDrawer.pop();
		std::cout << "(" << pos.l << "," << pos.c << ") ";
		queueDrawer.push(pos);
	}
	std::cout << "." << std::endl;

	return queueDrawer;
}
