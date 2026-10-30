
#include <iostream>
#include <fstream>

#include <Robot/Simulation/Playground.hpp>

#include <vendor/nlohmann/json.hpp>

#include <GLFW/glfw3.h>


Playground::Playground()
{
	agent_ = std::make_unique<AgentController>(this);
	view_  = std::make_unique<PlaygroundView> (this);
}

JSONParserStatus Playground::start(GLFWwindow * window, Directive const & instructions)
{
	std::cout << "Chargement des donness." << std::endl;

	JSONParserStatus status = loadData(instructions);
	if (status != JSONParserStatus::Ok)
		return status;

	std::cout << "Donnees chargees." << std::endl;

	agent_->start(script_.getRequests(), map_.generateInfo(), closet_.getCasierDepart());

	view_->init(map_, *agent_.get(), closet_, window);

	return status;
}

void Playground::update()
{
	agent_->tick();
}

void Playground::fixedUpdate(double deltaTime)
{
	if (!animationStack_.empty())
	{
		auto & currentAnimation = animationStack_.at(0);

		auto result = currentAnimation();
		if (result == Stop) animationStack_.erase(animationStack_.begin());
	}

	view_->update(deltaTime);
}

void Playground::draw()
{
	view_->draw();
}

bool Playground::agentIsOver()
{
	return agent_->requestsOver();
}

std::vector<Pas> Playground::steps()
{
	return agent_->returnSteps();
}

std::vector<Livraison> Playground::deliveries()
{
	return agent_->returnDeliveries();
}

std::string Playground::getScriptName()
{
	return script_.getName();
}

std::string Playground::getMapName()
{
	return map_.getName();
}

bool Playground::canMove()
{
	bool canMove = true;
	Pos2D pos = agent_->getPosition();
	if (!map_.isInside(pos))
	{
		// TODO: push to trace (view)
		canMove = false;
	}

	auto tile = map_.getTile(pos);
	if (tile.isSolid())
	{
		// TODO: push to trace (view)
		canMove = false;
	}
	return canMove;
}

void Playground::placeBackItem(Item returnedItem)
{
	closet_.putBackItem(returnedItem);
}

std::vector<Tile> Playground::see()
{
	std::vector<Orientation> const directions = { North, West, South, East };

	TileProperty wall;
	wall.setSolid(true);

	std::vector<Tile> result;
	Pos2D posAgent = agent_->getPosition();

	auto index = 0;
	for (auto & dir : directions)
	{
		auto iterator = result.begin() + index++;
		auto pos = posAgent + directionToPos(dir);

		if (!map_.isInside(pos))
		{
			result.emplace(iterator, Tile{wall, pos});
			continue;
		}

		result.emplace(iterator, map_.getTile(pos));
	}

	return result;
}

std::tuple<Emotion, Intensity> Playground::consult(std::string message)
{
	std::tuple<Emotion, Intensity> result = dictionary_.identifEmoIntens(message, dictionary_.getEntrees());
	return result;
}

std::optional<Item> Playground::search(Pos2D pos)
{
	return closet_.getItem(pos);
}

std::optional<Item> Playground::take(Pos2D pos)
{
	auto item = closet_.removeItem(pos);

	if (item)
	{
		onAgentTakeItem(item.value());
	}

	return item;
}

JSONParserStatus Playground::extractAndDetermineParsing(std::string const & path, int & parsingType, json & data)
{
	JSONParserStatus status = readJsonFile(path, data);
	if(status != JSONParserStatus::Ok)
		return status;

	if(!data.contains("format"))
		return JSONParserStatus::MissingField;

	std::string typeToFill =  data["format"];
	if(typeToFill == "robot-reconfort/scenario")
		parsingType = 0;
	else if(typeToFill == "robot-reconfort/carte")
		parsingType = 1;
	else if(typeToFill == "robot-reconfort/armoire")
		parsingType = 2;
	else if(typeToFill == "robot-reconfort/dictionnaire")
		parsingType = 3;
	else
		status = JSONParserStatus::WrongFormat;
	//std::cout << "Donnees determinees : " << parsingType << std::endl;

	return status;
}

template <class EnvElem>
JSONParserStatus Playground::extract(std::string const & path, EnvElem & toFill)
{
	json data;
	JSONParserStatus status = readJsonFile(path, data);
	if(status != JSONParserStatus::Ok)
		return status;
	
	status = Playground::parse<EnvElem>(data, toFill);

	return status;
}

template <class EnvElem>
JSONParserStatus Playground::parse(json data, EnvElem & toFill)
{
	return toFill.parseJSON(data);
}

JSONParserStatus Playground::conformScriptMap(Script script, Map map)
{
	if (script.getMapName() != map.getName())
		return JSONParserStatus::ConflictingRequirement;
	
	for (Request req : script.getRequests())
	{
		if (map.atResident(req.getIdResident()).name_ == "Non identifie")
			return JSONParserStatus::ConflictingRequirement;
	}
	return JSONParserStatus::Ok;
}

JSONParserStatus Playground::loadData(Directive const & instructions)
{
	json data;
	JSONParserStatus status = Playground::extract<Script>(instructions.script, script_);
	if (status != JSONParserStatus::Ok) 
		return status;

	status = Playground::extract<Map>(instructions.map, map_);
	if (status != JSONParserStatus::Ok) 
		return status;
	
	status = Playground::conformScriptMap(script_, map_);
	if (status != JSONParserStatus::Ok) 
		return status;

	// TODO : DEBATE do we use the name in the script file or the name in the instruction set ?
	std::string closetPath     = instructions.data + "/" + script_.getClosetName() + ".json";
	std::string dictionaryPath = instructions.data + "/dictionnaire.json";

	status = Playground::extract<ItemStash>(closetPath, closet_);
	if (status != JSONParserStatus::Ok) 
		return status;

	status = Playground::extract<Dictionary>(dictionaryPath, dictionary_);
	
	return status;
}

void Playground::onMapInit(std::vector<Tile> map, MapInfo info)
{

}

void Playground::onAgentMove(Pos2D oldPos, Pos2D newPos)
{
	animationStack_.emplace_back(view_->moveAgent(oldPos, newPos));
}

void Playground::onAgentTakeItem(Item item)
{
	animationStack_.emplace_back(view_->equipItem(item, closet_));
	if (closetOpen_)
	{
		closetOpen_ = false;
		onAgentStashClose();
	}
}

void Playground::onAgentGiveItem(Item item, Pos2D resident)
{
	animationStack_.emplace_back(view_->giveItem(resident));
}

void Playground::onAgentStashOpen()
{
	animationStack_.emplace_back(view_->openChest());
}

void Playground::onAgentStashClose()
{
	animationStack_.emplace_back(view_->closeChest());
}

void Playground::onAgentConsult()
{

}

void Playground::onAgentSelect(Pos2D old, Pos2D cursor)
{
	if (!closetOpen_)
	{
		closetOpen_ = true;
		onAgentStashOpen();
	}
	animationStack_.emplace_back(view_->moveChestCursor(old, cursor));
}
