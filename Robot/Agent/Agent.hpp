#pragma once

#include <Robot/Common/AgentCommandListener.hpp>
#include <Robot/Common/Item.hpp>

#include <functional>
#include <queue>
#include <optional>

struct Request
{
	uint32_t id_;
	std::string resident_;
	std::string message_;
};

enum JobStatus
{
	Stop,  // arrêter le job
	Yield, // continuer le job au prochain tick
	Swap,  // remplace frontJobs_ par backJobs_
	Break, // casse la chaine tout court
};

using Job = std::function<JobStatus()>;

class Agent
{
public:
	Agent(AgentCommandListener * listener);

	void start(std::initializer_list<Request> requests);
	void tick();

	Pos2D getPosition() const;
	void setPosition(Pos2D newPos);

	void pushJob(Job const & job);

private:
	Pos2D pos_;
	Pos2D dictionary_;
	Pos2D itemStash_;
	Pos2D currentResident_;

private:
	std::string wantedItem_;
	std::optional<Item> carriedItem_ {std::nullopt};
	
private:
	void updateMentalMap();

private:
	std::queue<Job> frontJobs_;
	std::queue<Job> backJobs_;
	bool pushSafe_ = true;

private:
	Job moveTowards(Pos2D pos);
	Job consult();
	Job takeItem();
	Job giveItem();
	Job endRequest();
	
private:
	AgentCommandListener * listener_;

};