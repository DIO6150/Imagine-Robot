#pragma once

#include <string>

enum class JSONParserStatus: int
{
	Ok                   = 0,
	MissingField         = 1,
	WrongType            = 2,
	WrongValue           = 3,
	UnknownProperty      = 4,
	IncoherentData       = 5,
};

enum class CommandStatus
{
	Ok,
	AgentMovementObstructed,
	AgentAlreadyCarryingObject,
	StashNotInRange,
	StashEmpty,
	ResidentNotInRange,
	OutOfBounds,
	GodSaidNo,
};

template<class T>
class CommandResult
{
public:
	CommandResult(CommandStatus status)
		: status_ {status}
	{

	}

	CommandResult(T value)
		: value_ {value}
	{
		
	}

	CommandStatus failureReason()
	{
		return status_;
	}

	bool operator==(CommandStatus const & status)
	{
		return status_ == status;
	}

	operator bool() { return status_ == CommandStatus::Ok; }
	T get() { return value_; }

private:
	CommandStatus status_ = CommandStatus::Ok;
	T value_;
};

template<>
class CommandResult<void>
{
public:
	CommandResult(CommandStatus status)
		: status_ {status}
	{

	}

	CommandStatus failureReason()
	{
		return status_;
	}

	bool operator==(CommandStatus const & status)
	{
		return status_ == status;
	}

	operator bool() { return status_ == CommandStatus::Ok; }

private:
	CommandStatus status_;
};

enum class AgentFailureType
{
	GivingItem,
	
};

inline std::string failureToString(AgentFailureType type)
{
	switch (type)
	{
	case AgentFailureType::GivingItem: return "Giving Item";
	default: return "Unkwown Failure";
	}
}