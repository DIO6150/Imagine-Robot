#pragma once

#include <string>
#include <iostream>

enum class JSONParserStatus: int
{
	Ok							= 0,
	IncorrectPath				= 1,
	FileNotFound				= 2,
	NotUTF8						= 3,
	InvalidJson					= 4,
	WrongFormat					= 5,
	WrongVersion				= 6,
	MissingField				= 7,
	WrongType					= 8,
	PositionOutOfGrid			= 9,
	GridNotRectangular			= 10,
	InacurrateDimensions		= 11,
	UnknownProperty				= 12,
	UnknownEmotion				= 13,
	UnknownIntensity 			= 14,
	InacurratePosition			= 15,
	ResidentSharedId			= 16,
	ConflictingRequirement		= 17,
	DrawerStartOutOfCloset		= 18,
	DrawerPositionOutOfCloset	= 19,
	DoubleDrawer				= 20,
	InaccurateDrawerPosition	= 21,
	WordTwiceDefined			= 22,
	ArrayEmotions				= 23,
	ArrayIntensity				= 24,
	ReadingError				= 25,
};

inline void printError(JSONParserStatus error)
{
	std::cerr << "Erreur " << static_cast<int>(error) << " : ";
	switch (error)
		{
			case JSONParserStatus::IncorrectPath :
				std::cerr << "Chemin illisible";
				break;
			case JSONParserStatus::FileNotFound :
				std::cerr << "Fichier introuvable";
				break;
			case JSONParserStatus::NotUTF8 :
				std::cerr << "Fichier non encode en UTF-8";
				break;
			case JSONParserStatus::WrongFormat :
				std::cerr << "Format different de celui attendu";
				break;
			case JSONParserStatus::WrongVersion :
				std::cerr << "Version different de celle attendue";
				break;
			case JSONParserStatus::InvalidJson :
				std::cerr << "JSON syntaxiquement invalide";
				break;
			case JSONParserStatus::MissingField :
				std::cerr << "Champ obligatoire absent";
				break;
			case JSONParserStatus::WrongType :
				std::cerr << "Champ du mauvais type";
				break;
			case JSONParserStatus::PositionOutOfGrid :
				std::cerr << "Position hors de la grille";
				break;
			case JSONParserStatus::GridNotRectangular :
				std::cerr << "Grille non rectangulaire";
				break;
			case JSONParserStatus::InacurrateDimensions :
				std::cerr << "Dimensions de grille non conformes";
				break;
			case JSONParserStatus::UnknownProperty :
				std::cerr << "Symbole de grille inconnu";
				break;
			case JSONParserStatus::UnknownEmotion :
				std::cerr << "Emotion inconnue";
				break;
			case JSONParserStatus::UnknownIntensity :
				std::cerr << "Intensite inconnue";
				break;
			case JSONParserStatus::InacurratePosition :
				std::cerr << "Position de grille non conforme aux coordonnees donnees";
				break;
			case JSONParserStatus::ResidentSharedId :
				std::cerr << "Deux residents possedent le meme identifiant";
				break;
			case JSONParserStatus::ConflictingRequirement :
				std::cerr << "Carte en entree differente de la carte attendue";
				break;
			case JSONParserStatus::DrawerStartOutOfCloset :
				std::cerr << "Casier de depart hors de l'armoire";
				break;
			case JSONParserStatus::DrawerPositionOutOfCloset :
				std::cerr << "Casier hors de l'armoire";
				break;
			case JSONParserStatus::DoubleDrawer :
				std::cerr << "Deux casier possedent la meme emotion et intensite";
				break;
			case JSONParserStatus::InaccurateDrawerPosition :
				std::cerr << "Etiquette emotion ou intensite non conforme aux coordonnees donnees";
				break;
			case JSONParserStatus::WordTwiceDefined :
				std::cerr << "Mot defini pour des emotions ou intensites differentes";
				break;
			case JSONParserStatus::ArrayEmotions :
				std::cerr << "Surplus ou manque d'emotion dans la liste";
				break;
			case JSONParserStatus::ArrayIntensity :
				std::cerr << "Surplus ou manque d'intensite dans la liste";
				break;
			case JSONParserStatus::ReadingError :
				std::cerr << "Erreur survenue durant la lecture du fichier";
				break;
			default :
				std::cerr << "erreur inconnue ou fichier de test conforme (0)";
		}
		std::cerr << ". Pas de trace ecrite\n" << std::endl;
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
