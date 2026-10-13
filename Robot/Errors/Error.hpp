#pragma once

#include <string>

enum class JSONParserStatus: int
{
	Ok                   = 0,
	IncorrectArgument    = 1,
	MissingField         = 2,
	WrongType            = 3,
	WrongValue           = 4,
	UnknownProperty      = 5,
	UnknownEmotion       = 6,
	UnknownIntensity     = 7,
	IncoherentData       = 8,
};

enum class CommandStatus
{
	Ok,
	AgentMovementObstructed,
	AgentAlreadyCarryingObject,
	UnindentifiedEmotion,
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
	CommandResult(T value)
		: value_ {value}
	{
		
	}

	CommandResult(CommandStatus status, T value)
		: status_ {status}
		, value_ {value}
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
	Move,
	
};

inline std::string failureToString(AgentFailureType type)
{
	switch (type)
	{
	case AgentFailureType::GivingItem: return "Giving Item";
	case AgentFailureType::Move: return "Move";
	default: return "Unkwown Failure";
	}
}
