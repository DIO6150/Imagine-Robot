
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
	if (currentReq_ >= requestList_.size())
		return;

	std::cout << "\nCurrent Objective : ";
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
	return currentReq_ >= requestList_.size();
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
	if (!noRequestsLeft())
	{
		std::cout << "\nRequest n" << requestList_[currentReq_].getId() << std::endl;
		destinationReached_ = false;
		for (auto dir : directions)
		{
			currentObjective_ = ((pos_ + directionToPos(dir) == mapInfo_.dictionaryPos) ? Objective::InterpretRequest : currentObjective_);
		}
		if (currentObjective_ == Objective::ReachDictionary)
		{
			destination_ = mapInfo_.dictionaryPos;
			replan();
		}
	}
	else
		std::cout << "L'agent a termine ses requetes." << std::endl;
	deliverySuccess_ = false;
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
	destination_ = mapInfo_.dictionaryPos;
	deliverySuccess_ = false;
	itemName_ = "vide";
	currentSearchedEmotion_ = Emotion::Unidentified;
	currentSearchedIntensity_ = Intensity::Unidentified;
	currentObjective_ = Objective::ReachDictionary;
	replan();

	std::cout << "\nTotal request : " << requestList_.size() << std::endl;
	std::cout << "\nRequest n" << requestList_[currentReq_].getId() << std::endl;
}

bool Brain::perceptionSurroundings(std::vector<Tile> & percepTiles)
{
	auto nwse = body_->see();

	if (!nwse)
	{
		std::cout << "ERROR CANT RETRIEVE PERCEPTION\n";
		nextRequest("echec perception");
		simulationProgress();
		return false;
	}

	percepTiles = nwse.get();
	return true;
}

bool Brain::perceptionCurrentDrawer(std::string & percepDrawer)
{
	auto result = body_->openCurrentDrawer();

	if (!result)
	{
		std::cout << "ERROR CANT RETRIEVE DRAWER CONTENT\n";
		nextRequest("echec perception casier");
		simulationProgress();
		return false;
	}
	
	percepDrawer = result.get();
	std::cout << "Casier " << "(" << casier_.l << "," << casier_.c << ") : " << percepDrawer << std::endl;
	return true;
}

void Brain::updateMentalMap(std::vector<Tile> perception)
{
	for (auto i = 0; i < directions.size(); ++i)
	{
		auto tile = perception[i];
		
		if (!inRect(tile.pos_, 0, mapInfo_.height, 0, mapInfo_.width))
			continue;

		mentalMap_[calculateIndex(tile.pos_, mapInfo_.width)] = tile;
		currentPerception_[directions[i]] = tile;
		//std::cout << tile.pos_.l << "," << tile.pos_.c << " : " << mentalMap_[calculateIndex(tile.pos_, mapInfo_.width)].isSolid() << std::endl;
	}
}

void Brain::updateDrawer(std::string content)
{
	drawersContainsItem_[casier_.l*8 + casier_.c] = content;
}

bool Brain::keepSearching(Orientation & dirDrawer, bool & shouldTake)
{
	destination_ = drawerCheckQueue_.front();
	while(drawersContainsItem_[destination_.l*8 + destination_.c] == "vide")
	{
		drawerCheckQueue_.pop();
		destination_ = ((drawerCheckQueue_.empty()) ? Pos2D {-1, -1} : drawerCheckQueue_.front());
	}

	if (drawerCheckQueue_.empty())
	{
		body_->onAgentFail(AgentFailureType::Search, "L'armoire ne contient pas d'objet adapte. Passage à la requete suivante.");
		nextRequest("armoire vide");
		return false;
	}

	if (casier_ == destination_) {
		if (drawersContainsItem_[casier_.l*8 + casier_.c] != "vide")
		{
			currentObjective_ = Objective::TakeItem;
			simulationProgress();
			std::queue<Pos2D> empty;
			std::swap(drawerCheckQueue_, empty);
			shouldTake = true;
			return false;
		}
		else
			drawerCheckQueue_.pop();
	}
	
	if(destination_.l != casier_.l)
	{
		//std::cout << "Nord/Sud" << std::endl;
		dirDrawer = findDirection(destination_, casier_);
	}
	else
	{
		//std::cout << "Est/Ouest" << std::endl;
		if((destination_.c - casier_.c + 8)%8 < 4)
			dirDrawer = Orientation::East;
		else
			dirDrawer = Orientation::West;
	}
	return true;
}

void Brain::checkPathEnd(bool & shouldMove)
{
	destinationReached_ = false;
	for (auto dir : directions)
	{
		destinationReached_ = ((pos_ + directionToPos(dir) == destination_) ? true : destinationReached_);
	}

	if (destinationReached_)
	{
		// target reached
		if (currentObjective_ == Objective::ReachDictionary)
			currentObjective_ = Objective::InterpretRequest;
		if (currentObjective_ == Objective::ReachCloset)
		{
			currentObjective_ = Objective::FindItem;
			if(itemName_ != "vide")
			{
				body_->placeBackItem(mapInfo_.stashPos);
				itemName_ = "vide";
			}
			drawerCheckQueue_ = closetPath();
		}
		if (currentObjective_ == Objective::ReachResident)
			currentObjective_ = Objective::GiveItem;
		destinationReached_ = false;
		shouldMove = false;
	}
}

void Brain::replan()
{
	currentPath_ = Brain::shortestPath(pos_, destination_, mentalMap_, mapInfo_.width);
	if (currentPath_.empty())
	{
		body_->onAgentFail(AgentFailureType::Move, "La destination requise n'est pas accessible. Passage à la requete suivante");
		if (currentObjective_ == Objective::ReachDictionary)
			nextRequest("dictionnaire inaccessible");
		if (currentObjective_ == Objective::ReachCloset)
			nextRequest("armoire inaccessible");
		if (currentObjective_ == Objective::ReachResident)
			nextRequest("resident inaccessible");
	}
}

void Brain::prepareMove(DeducedStates & reasoning)
{
	Pos2D curStep;
	std::queue<Pos2D> temp = currentPath_;
	for(int istep = 0; istep < currentPath_.size(); istep++)
	{
		curStep = temp.front();
		temp.pop();
		//std::cout << curStep.l << "," << curStep.c << " : " << mentalMap_[calculateIndex(curStep, mapInfo_.width)].isSolid() << std::endl;
		reasoning.needReplan = ((mentalMap_[calculateIndex(curStep, mapInfo_.width)].isSolid()) ? true : reasoning.needReplan);
	}

	//std::cout << reasoning.needReplan << std::endl;
	if (reasoning.needReplan)
		replan();

	reasoning.directionToTake = findDirection(currentPath_.front(), pos_); // findDirection is maybe wrong
}

void Brain::decideNextAction(DeducedStates & reasoning)
{	
	reasoning.shouldMove = (currentObjective_ == Objective::ReachDictionary
		|| currentObjective_ == Objective::ReachCloset
		|| currentObjective_ == Objective::ReachResident);
	
	// target reached or no valid path
	if (reasoning.shouldMove && currentPath_.empty())
		checkPathEnd(reasoning.shouldMove);

	switch (currentObjective_)
	{
		case Objective::InterpretRequest:
			reasoning.shouldConsult = true;
			break;
		
		case Objective::FindItem:
			reasoning.shouldSearch = keepSearching(reasoning.directionToTake, reasoning.shouldTakeItem);
			break;
		
		case Objective::TakeItem:
			reasoning.shouldTakeItem = true;
			break;

		case Objective::GiveItem:
			reasoning.shouldGiveItem = true;
			break;
		
		default:
			reasoning.shouldMove = true;
			prepareMove(reasoning);
			break;
	}
}

void Brain::move(Orientation direction)
{
	addStep(direction);

	auto result = body_->move(direction);

	if (result)
	{
		pos_ += directionToPos(direction); // ajust mental position
		currentPath_.pop();
	}
	else
	{
		body_->onAgentFail(AgentFailureType::Move, "ERROR MOVE FAILED");
		nextRequest("echec execution AVANCER");
		return;
	}
}

void Brain::consult()
{
	addStep(Orientation::North);
		
	auto result = body_->consult();

	if (!result)
	{
		body_->onAgentFail(AgentFailureType::Consult, "ERROR CONSULT FAILED");
		nextRequest("echec execution CONSULTER");
		return;
	}
	
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
		destination_ = mapInfo_.stashPos;
		replan();
	}
}

void Brain::take()
{
	addStep(Orientation::North);

	itemName_ = drawersContainsItem_[casier_.l*8 + casier_.c];
	auto result = body_->take();

	if (!result)
	{
		body_->onAgentFail(AgentFailureType::Take, "ERROR TAKE FAILED");
		nextRequest("echec execution PRENDRE");
		itemName_ = "vide";
		carryItem_ = false;
		return;
	}

	carryItem_ = true;
	drawersContainsItem_[casier_.l*8 + casier_.c] = "vide";
	std::cout << "item taken : " << itemName_ << std::endl;
	currentObjective_ = Objective::ReachResident;
	destination_ = mapInfo_.residents_[requestList_[currentReq_].getIdResident()].pos_;
	replan();
}

void Brain::takeAction(DeducedStates reasoning)
{
	if (reasoning.shouldMove)
		move(reasoning.directionToTake);
	else if (reasoning.shouldConsult)
		consult();
	else if (reasoning.shouldSearch)
	{
		addStep(reasoning.directionToTake);

		casier_ = casier_ + directionToPos(reasoning.directionToTake);
		casier_.c = (casier_.c + 8)%8;
		auto result = body_->search(reasoning.directionToTake);

		if (!result)
		{
			body_->onAgentFail(AgentFailureType::Search, "ERROR SEARCH FAILED");
			nextRequest("echec execution CHERCHER");
			itemName_ = "vide";
			carryItem_ = false;
			return;
		}
	}
	else if (reasoning.shouldTakeItem)
		take();
	else if (reasoning.shouldGiveItem)
	{
		addStep(Orientation::North);

		auto result = body_->give(destination_);

		if (!result)
		{
			body_->onAgentFail(AgentFailureType::Give, "ERROR GIVE FAILED");
			nextRequest("echec execution DONNER");
			return;
		}
		nextRequest("null");
		itemName_ = "vide";
		carryItem_ = false;
	}
}

void Brain::tick()
{
	simulationProgress();
	DeducedStates reasoning;

	// PERCEVOIR
	std::vector<Tile> percepTiles;
	std::string percepDrawer;

	bool percepTileDone = perceptionSurroundings(percepTiles);
	bool percepDrawerDone = false;

	for (auto dir : directions)
		percepDrawerDone = ((pos_ + directionToPos(dir) == mapInfo_.stashPos) ? true : percepDrawerDone);

	if(percepDrawerDone)
		percepDrawerDone = perceptionCurrentDrawer(percepDrawer);

	// MEMORISER
	if(percepTileDone)
		updateMentalMap(percepTiles);

	if(percepDrawerDone)
		updateDrawer(percepDrawer);

	// DECIDER
	decideNextAction(reasoning);

	// AGIR
	takeAction(reasoning);

	nStepsRequest_++;
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
				std::cout << "Chemin trouve :";
				while(currentPos != start)
				{
					//std::cout << currentPos.l << "," << currentPos.c << std::endl;
					toReverse.push_back(currentPos);
					currentPos = previous.at(currentPos);
				}

				std::swap(queue, empty);
				for(int i = toReverse.size()-1; i >= 0; i--)
					queue.push(toReverse[i]);
				
				std::queue<Pos2D> temp = queue;
				Pos2D front = temp.front();
				for (int i = 0; i < queue.size(); i++) {
					temp.pop();
					std::cout << " (" << front.l << "," << front.c << ")";
					front = temp.front();
				}
				std::cout << ".\n" << std::endl;
				
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
