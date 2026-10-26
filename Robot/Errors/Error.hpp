#pragma once

#include <string>
#include <iostream>

enum class JSONParserStatus: int
{
	Ok							= 0,
	FileNotFound				= 1,
	NotUTF8						= 2,
	IncorrectPath				= 3,
	WrongFormat					= 4,
	WrongVersion				= 5,
	InvalidJson					= 6,
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

inline void printError(JSONParserStatus error)
{
	std::cout << "Erreur " << static_cast<int>(error) << " : ";
	switch (error)
		{
			case JSONParserStatus::FileNotFound :
				std::cout << "Fichier absent";
				break;
			case JSONParserStatus::NotUTF8 :
				std::cout << "Fichier non encodé en UTF-8";
				break;
			case JSONParserStatus::IncorrectPath :
				std::cout << "Fichier non lisible";
				break;
			case JSONParserStatus::WrongFormat :
				std::cout << "Format inattendu";
				break;
			case JSONParserStatus::WrongVersion :
				std::cout << "Version inattendu";
				break;
			case JSONParserStatus::InvalidJson :
				std::cout << "JSON syntaxiquement invalide";
				break;
			case JSONParserStatus::MissingField :
				std::cout << "Champ obligatoire absent";
				break;
			case JSONParserStatus::WrongType :
				std::cout << "Champ du mauvais type";
				break;
			case JSONParserStatus::WrongValue :
				std::cout << "Champ obligatoire avec une valeur inadequate";
				break;
			case JSONParserStatus::PositionOutOfGrid :
				std::cout << "Position hors de la grille";
				break;
			case JSONParserStatus::GridNotRectangular :
				std::cout << "Grille non rectangulaire";
				break;
			case JSONParserStatus::InacurrateDimensions :
				std::cout << "Dimensions de grille non conformes";
				break;
			case JSONParserStatus::UnknownProperty :
				std::cout << "Symbole de grille inconnu";
				break;
			case JSONParserStatus::UnknownEmotion :
				std::cout << "Emotion inconnue";
				break;
			case JSONParserStatus::UnknownIntensity :
				std::cout << "Intensite inconnue";
				break;
			case JSONParserStatus::InacurratePosition :
				std::cout << "Position de grille non conforme";
				break;
			case JSONParserStatus::ResidentSharedId :
				std::cout << "Deux residents possedent le meme identifiant";
				break;
			case JSONParserStatus::ConflictingRequirement :
				std::cout << "Carte en entree differente de carte attendue";
				break;
			case JSONParserStatus::DrawerStartOutOfCloset :
				std::cout << "Casier de depart hors de l'armoire";
				break;
			case JSONParserStatus::DrawerPositionOutOfCloset :
				std::cout << "Casier hors de l'armoire";
				break;
			case JSONParserStatus::DoubleDrawer :
				std::cout << "Deux casier possedent la meme emotion et intensite";
				break;
			case JSONParserStatus::InaccurateDrawerPosition :
				std::cout << "Position de casier non conforme";
				break;
			case JSONParserStatus::WordTwiceDefined :
				std::cout << "Mot defini pour des emotions ou intensites differentes";
				break;
			default :
				std::cout << "erreur inconnue";
		}
		std::cout << ". Pas de trace ecrite" << std::endl;
}

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
	Move,
	Consult,
	Search,
};

inline std::string failureToString(AgentFailureType type)
{
	switch (type)
	{
		case AgentFailureType::Move: return "AVANCER";
		case AgentFailureType::Consult: return "CONSULTER";
		case AgentFailureType::Search: return "CHERCHER";
		default: return "Aucun echec d'action";
	}
}
