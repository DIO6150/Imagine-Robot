#pragma once

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

	operator bool() { return status_ == CommandStatus::Ok; }

private:
	CommandStatus status_;
};