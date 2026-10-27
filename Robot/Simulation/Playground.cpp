
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

	view_->init(map_);

	return status;
}

void Playground::tick()
{
	agent_->tick();

	if (!animationStack_.empty())
	{
		auto & currentAnimation = animationStack_.at(0);

		auto result = currentAnimation();
		if (result == Stop) animationStack_.erase(animationStack_.begin());
	}

	view_->update();
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

std::string Playground::search(Pos2D posDrawer)
{
	return closet_.getItem(posDrawer);
}

bool Playground::take(Pos2D posDrawer, std::string itemName)
{
	return closet_.removeItem(posDrawer, itemName);
}

template <class EnvElem>
JSONParserStatus Playground::parse(std::string const & path, EnvElem & toFill)
{
	std::ifstream stream {path};
	if (!stream.is_open())
        return JSONParserStatus::FileNotFound;
	json data   = json::parse(stream);
	auto status = toFill.parseJSON(data);

	stream.close();

	return status;
}

JSONParserStatus Playground::loadData(Directive const & instructions)
{
	JSONParserStatus status = Playground::parse<Script>(instructions.script, script_);
	if (status != JSONParserStatus::Ok) 
		return status;

	// TODO : DEBATE do we use the name in the script file or the name in the instruction set ?
	std::string mapPath        = instructions.map;
	std::string closetPath     = instructions.data + "/" + script_.getClosetName() + ".json";
	std::string dictionaryPath = instructions.data + "/dictionnaire.json";

	status = Playground::parse<Map>(mapPath, map_);
	if (status != JSONParserStatus::Ok) 
		return status;
	if (script_.getMapName() != map_.getName())
		return JSONParserStatus::ConflictingRequirement;
	for (Request req : script_.getRequests())
	{
		if (map_.atResident(req.getIdResident()).name_ == "Non identifie")
			return JSONParserStatus::ConflictingRequirement;
	}

	status = Playground::parse<ItemStash>(closetPath, closet_);
	if (status != JSONParserStatus::Ok) 
		return status;

	status = Playground::parse<Dictionary>(dictionaryPath, dictionary_);
	
	return status;
}

void Playground::onMapInit(std::vector<Tile> map, MapInfo info)
{

}

void Playground::onAgentMove(Pos2D newPos)
{

}

void Playground::onAgentTakeItem(Item item)
{

}

void Playground::onAgentGiveItem(Item item)
{

}

void Playground::onAgentStashOpen ()
{

}

void Playground::onAgentStashClose()
{

}

void Playground::onAgentConsult()
{

}

void Playground::onAgentSelect(Pos2D cursor)
{

}
