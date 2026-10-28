
#include <Robot/Parsers/ClosetParser.hpp>

using json = nlohmann::json;

ClosetParser::ClosetParser(ItemStash & stash, json const & object)
{
	JSONParser parser {
		{"format", "version", "nom", "emotions", "intensites", "casier_depart", "casiers"},
		{
		Field::validate("format",
			[&stash](json const & object) 
			{ 
				if (!object.is_string())
					return JSONParserStatus::WrongType;
				if (object.get<std::string>() != stash.FORMAT_)
					return JSONParserStatus::WrongFormat;
				return JSONParserStatus::Ok;
			}
		),
		Field::validate("version",
			[&stash](json const & object) 
			{ 
				if (!object.is_number_integer())
					return JSONParserStatus::WrongType;
				if (object.get<int>() != stash.VERSION_)
					return JSONParserStatus::WrongVersion;
				return JSONParserStatus::Ok;
			}
		),
		Field::validate("nom", JSONParser::verifySimpleString),
		Field::fillNoError("nom",
			[&stash](json const & object) { stash.name_ = object.get<std::string>(); }
		),
		Field::validate("emotions",
			[&stash](json const & object) 
			{ 
				if (!object.is_array())
					return JSONParserStatus::WrongType;

				int size = object.size();
				if (size != emotionToString.size())
					return JSONParserStatus::ArrayEmotions;

				for (int i = 0; i < size; i++)
				{
					if (!object.at(i).is_string())
						return JSONParserStatus::WrongType;
					if (std::count(emotionToString.begin(), emotionToString.end(), object.at(i).get<std::string>()) == 0)
						return JSONParserStatus::UnknownEmotion;
				}
				return JSONParserStatus::Ok;
			}
		),
		Field::fillNoError("emotions",
			[&stash](json const & object)
			{
				for (auto const & nested : object)
				{
					auto nom_emotion = nested.get<std::string>();
					Emotion emotion;

					if (nom_emotion == "joie")              emotion = Emotion::Joy;
					else if (nom_emotion == "confiance")    emotion = Emotion::Confidence;
					else if (nom_emotion == "peur")         emotion = Emotion::Fear;
					else if (nom_emotion == "surprise")     emotion = Emotion::Surprised;
					else if (nom_emotion == "tristesse")    emotion = Emotion::Sadness;
					else if (nom_emotion == "degout")       emotion = Emotion::Disgust;
					else if (nom_emotion == "colere")       emotion = Emotion::Anger;
					else if (nom_emotion == "anticipation") emotion = Emotion::Anticipation;

					stash.stringToEmotions_.emplace(nom_emotion, emotion);
				}
			}
		),
		Field::validate("intensites",
			[&stash](json const & object) 
			{ 
				if (!object.is_array())
					return JSONParserStatus::WrongType;

				int size = object.size();
				if (size != intensityToString.size())
					return JSONParserStatus::ArrayIntensity;

				for (int i = 0; i < size; i++)
				{
					if (!object.at(i).is_string())
						return JSONParserStatus::WrongType;
					if (std::count(intensityToString.begin(), intensityToString.end(), object.at(i).get<std::string>()) == 0)
						return JSONParserStatus::UnknownIntensity;
				}
				return JSONParserStatus::Ok;
			}
		),
		Field::fillNoError("intensites",
			[&stash](json const & object)
			{
				for (auto const & nested : object)
				{
					auto nom_intensite = nested.get<std::string>();
					Intensity intensity;

					if (nom_intensite == "faible")       intensity = Intensity::Small;
					if (nom_intensite == "moyenne") intensity = Intensity::Mid;
					if (nom_intensite == "forte")   intensity = Intensity::Strong;

					stash.stringToIntensities_.emplace(nom_intensite, intensity);
				}
			}
		),
		Field::validate("casier_depart",
			[&stash](json const & object) 
			{ 
				JSONParserStatus status = JSONParser::verifySimplePos(object);
				if (status != JSONParserStatus::Ok)
					return status;

				int ligne = object.at(0).get<int>();
				int colonne = object.at(1).get<int>();
				if (colonne < 0 || colonne > 7 || ligne < 0 || ligne > 2)
					return JSONParserStatus::DrawerStartOutOfCloset;
				return JSONParserStatus::Ok;
			}
		),
		Field::fillNoError("casier_depart",
			[&stash](json const & object)
			{
				Pos2D drawerStart; // obviously should check if its a list and if it has size == 2
				drawerStart.l = object.at(0).get<int>();
				drawerStart.c = object.at(1).get<int>();

				stash.casierDepart_ = drawerStart;
			}
		),
		Field::branch("casiers",
			{
				Field::validate("ligne",
					[&stash](json const & object) 
					{ 
						if (!object.is_number_integer())
							return JSONParserStatus::WrongType;

						if (object.get<int>() < 0 || object.get<int>() > 2)
							return JSONParserStatus::DrawerPositionOutOfCloset;
						
						return JSONParserStatus::Ok;
					}
				),
				Field::validate("colonne",
					[&stash](json const & object) 
					{ 
						if (!object.is_number_integer())
							return JSONParserStatus::WrongType;

						if (object.get<int>() < 0 || object.get<int>() > 7)
							return JSONParserStatus::DrawerPositionOutOfCloset;
						
						return JSONParserStatus::Ok;
					}
				),
				Field::validate("emotion",
					[&stash](json const & object) 
					{ 
						if (!object.is_string())
							return JSONParserStatus::WrongType;

						if (std::count(emotionToString.begin(), emotionToString.end(), object.get<std::string>()) == 0)
								return JSONParserStatus::UnknownEmotion;

						return JSONParserStatus::Ok;
					}
				),
				Field::validate("intensite",
					[&stash](json const & object) 
					{ 
						if (!object.is_string())
							return JSONParserStatus::WrongType;

						if (std::count(intensityToString.begin(), intensityToString.end(), object.get<std::string>()) == 0)
								return JSONParserStatus::UnknownIntensity;
							
						return JSONParserStatus::Ok;
					}
				),
				Field::validate("objet",
					[&stash](json const & object) 
					{ 
						if (((!object.is_string() || object.empty()) && !object.is_null()))
							return JSONParserStatus::WrongType;
						return JSONParserStatus::Ok;
					}
				)
			}
		),
		Field::validate("casiers",
			[&stash](json const & object) 
			{ 
				if (!object.is_array() || object.size() != 24)
					return JSONParserStatus::WrongType;
				
				for (auto const & nested : object)
				{
					if(!nested.is_object())
						return JSONParserStatus::WrongType;

					if(!nested.contains("ligne") || !nested.contains("colonne") || !nested.contains("emotion") 
					|| !nested.contains("intensite") || !nested.contains("objet"))
						return JSONParserStatus::MissingField;
				}
				
				return JSONParserStatus::Ok;
			}
		),
		Field::fill("casiers",
			[&stash](json const & object)
			{
				for (auto const & nested : object)
				{
					Drawer drawer;

					drawer.location_.c   = nested.at("colonne").get<int>();
					drawer.location_.l   = nested.at("ligne").get<int>();

					for (Drawer dr : stash.drawers_)
					{
						if (dr.location_.c == drawer.location_.c && dr.location_.l == drawer.location_.l)
							return JSONParserStatus::DoubleDrawer;
					}

					std::string emotionName_ = nested.at("emotion").get<std::string>();
					drawer.emotion_ = stash.stringToEmotions_[emotionName_];
					std::string intensityName_ = nested.at("intensite").get<std::string>();
					drawer.intensity_ = stash.stringToIntensities_[intensityName_];

					if (drawer.location_.c != emotionToInt(drawer.emotion_) || drawer.location_.l != intensityToInt(drawer.intensity_))
						return JSONParserStatus::InaccurateDrawerPosition;

					drawer.item_ = ((nested.at("objet").is_null()) ? "vide" : nested.at("objet").get<std::string>());
				
					stash.drawers_.push_back(drawer);
				}
				return JSONParserStatus::Ok;
			}
		)
	}
	};

	result = parser.fillFields(object);
}
