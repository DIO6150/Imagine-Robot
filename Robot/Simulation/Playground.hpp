#pragma once

#include <iostream>

#include <Robot/Simulation/Dictionary.hpp>
#include <Robot/Simulation/Directive.hpp>
#include <Robot/Simulation/ItemStash.hpp>
#include <Robot/Simulation/Map.hpp>
#include <Robot/Simulation/Script.hpp>

#include <Robot/Common/PlaygroundAgentListener.hpp>
#include <Robot/Common/PlaygroundViewListener.hpp>
#include <Robot/Common/TraceElement.hpp>

#include <Robot/Render/PlaygroundView.hpp>

#include <Robot/Agent/AgentController.hpp>

#include <Robot/Utils/Animation.hpp>

class GLFWwindow;

class Playground : public PlaygroundViewListener, public PlaygroundAgentListener
{
public:
	Playground();
	
	// Presenter Code
	JSONParserStatus start(GLFWwindow * window, Directive const & instructions);
	void update();
	void fixedUpdate(double deltaTime);
	void draw();

public:
	std::vector<Pas> steps();
	std::vector<Livraison> deliveries();
	
	std::string getScriptName();
	std::string getMapName();

	JSONParserStatus loadData(Directive const & instructions);

	template <class EnvElem>
	static JSONParserStatus parse(std::string const & path, EnvElem & toFill);
	
	void onMapInit(std::vector<Tile> map, MapInfo info) override;

	void onAgentMove(Pos2D newPos) override;

	void onAgentTakeItem(Item item) override;
	void onAgentGiveItem(Item item) override;

	void onAgentStashOpen () override;
	void onAgentStashClose() override;

	void onAgentConsult() override;

	void onAgentSelect(Pos2D cursor) override;

	bool agentIsOver();
	
private:

	bool canMove() override;
	std::vector<Tile> see() override;
	std::tuple<Emotion, Intensity> consult(std::string message) override;
	std::string search(Pos2D posDrawer) override;
	bool take(Pos2D posDrawer, std::string itemName) override;
	
private:
	JSONParserStatus parse(std::string const & path, std::string toFill);

private:
	Script script_;
	Map map_;
	ItemStash closet_;
	Dictionary dictionary_;

	GLFWwindow * window_;
	std::vector<Animation> animationStack_;

private:
	std::unique_ptr<AgentController> agent_;
	std::unique_ptr<PlaygroundView> view_;
};
