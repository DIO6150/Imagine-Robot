#pragma once

#include <string>

enum class JSONParserStatus: int
{
	Ok							= 0,
	FileNotFound				= 1,
	NotUTF8						= 2,
	IncorrectPath				= 3,
	WrongFormat					= 4,
	WrongVersion				= 5,
	IncorrectArgument			= 6,
	MissingField				= 7,
	WrongType					= 8,
	WrongValue					= 9,
	PositionOutOfGrid			= 10,
	GridNotRectangular			= 11,
	InacurrateDimensions		= 12,
	UnknownProperty 			= 13,
	UnknownEmotion				= 14,
	UnknownIntensity			= 15,
	InacurratePosition			= 16,
	ResidentSharedId			= 17,
	ConflictingRequirement		= 18,
	DrawerStartOutOfCloset		= 19,
	DrawerPositionOutOfCloset	= 20,
	DoubleDrawer				= 21,
	InaccurateDrawerPosition	= 22,
	WordTwiceDefined			= 23,
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
