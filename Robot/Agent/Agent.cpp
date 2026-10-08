
#include <Robot/Agent/Agent.hpp>

Agent::Agent(AgentCommandListener * listener)
	: listener_ {listener}
{

}

void Agent::start(std::initializer_list<Request> requests)
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

void Agent::tick()
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

Pos2D Agent::getPosition() const
{
	return pos_;
}

void Agent::setPosition(Pos2D newPos)
{
	pos_ = newPos;
}

void Agent::pushJob(Job const & job)
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

Job Agent::moveTowards(Pos2D pos)
{
	return [this, pos]()
	{
		// code here

		return Stop;
	};
}

Job Agent::consult()
{
	return [this]()
	{
		auto result = listener_->consult();
		if (!result) return Break;
		// TODO : do something with the result
		return Stop;
	};
}

Job Agent::takeItem()
{
	return [this]()
	{
		auto result = listener_->take(wantedItem_);
		if (!result) return Break;

		carriedItem_ = result.get();

		return Stop;
	};
}

Job Agent::giveItem()
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

Job Agent::endRequest()
{
	return []()
	{
		return Stop;
	};
}
