#pragma once

#include <memory>

#include <Robot/Errors/Error.hpp>

#include <Robot/Simulation/Directive.hpp>

class Playground;

enum class CommandError
{
	Ok,
	AgentMovementObstructed,
	AgentAlreadyCarryingObject,
	StashNotInRange,
	StashEmpty,
	ResidentNotInRange,
	GodSaidNo,
};

enum class Orientation
{
	North,
	East,
	West,
	South
};

struct AgentTileView
{
	Tile north_;
	Tile east_;
	Tile west_;
	Tile south_;
};

struct AgentCommandListener
{
	virtual CommandError move(Orientation direction) = 0;

	virtual CommandError consult() = 0;
	virtual CommandError search(Orientation direction) = 0;

	virtual CommandError take(std::string name) = 0;
	virtual CommandError give() = 0;

	virtual AgentTileView see() = 0;

	virtual void wait() = 0;
};

class Agent
{
public:
	Agent(AgentCommandListener *);

private:
	AgentCommandListener * listener_;
};

#include <functional>

using AnimationID = int;
using AnimationContext = int;

class Playground : public AgentCommandListener
{
public:

	// Presenter Code

	void init();
	void update();
	void draw(); // relevant only if rendering with a graphic api (i mean we could write the log to a stream and then flush that steam in draw() but whatever)

	// Agent Listener Code
	CommandError move(Orientation direction) override
	{
		// 
		Pos2D newPos = agent_->getPos() + orientationToPos(direction);
		if (!map_->inRange(newPos)) return CommandError::AgentMovementObstructed;
		agent_->setPosition(newPosition);

		// view commands here
		renderer_->moveAgent(newPos);
	}

	CommandError consult() override
	{

	}

	CommandError search(Orientation direction) override
	{

	}


	CommandError take(std::string name) override
	{

	}

	CommandError give() override
	{

	}


	AgentTileView see() override
	{

	}

	void wait() override
	{

	}

	// View Code
	// the view usually returns a function producing function
	// so that events can be sequenced correctly
	// ie (IN THE CORRESPONDING VIEW CLASS):

	//std::function<AnimationID ()> moveAgent()
	//{
	//	return [this] () -> AnimationID
	//	{
	//		AnimationID id = animationSequencer_.generate(
	//			[=, this](AnimationID id, double deltaTime)
	//			{
	//				someOperation(someVar);
	//
	//				if (someVar == someValue)
	//					return AnimationState::Stop;
	//
	//				return AnimationState::Yield;
	//			}
	//		);
	//
	//		animationSequencer_.start(id, 0.2f);
	//	};
	//}
	// however this is only relevant in a rendering context, if our means of showing change is just the terminal (which is just a bummer if thats the case),
	// then no need to fancy ourselves with this mechanism (unless we want information to not appear instantaniously on the screen)
	// btw as it is a simulation, user inputs are kinda limited to pause the simulation :/

private:
	std::unique_ptr<Map> map_;
	std::unique_ptr<Agent> agent_;

	std::unique_ptr<PlaygroundRenderer> renderer_;
};
