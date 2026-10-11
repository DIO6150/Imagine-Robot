
#include <Robot/Parsers/ClosetParser.hpp>


#include <Robot/Parsers/MapParser.hpp>
#include <Robot/Simulation/ItemStash.hpp>

#include <Robot/Utils/JSONParser.hpp>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

ClosetParser::ClosetParser(ItemStash & stash, json const & object)
{
	JSONParser parser {
		Field::validate("format",
			[&stash](json const & object) { return object.get<std::string>() == stash.FORMAT_; }
		),
		Field::validate("version",
			[&stash](json const & object) { return object.get<int>() == stash.VERSION_; }
		),
		Field::fillNoError("nom",
			[&stash](json const & object) { stash.name_ = object.get<std::string>(); }
		),
		Field::fill("emotions",
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
					else return JSONParserStatus::UnknownEmotion;

					stash.stringToEmotions_.emplace(nom_emotion, emotion);
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("intensites",
			[&stash](json const & object)
			{
				for (auto const & nested : object)
				{
					auto nom_intensite = nested.get<std::string>();
					Intensity intensity;

					if (nom_intensite == "faible")       intensity = Intensity::Small;
					else if (nom_intensite == "moyenne") intensity = Intensity::Mid;
					else if (nom_intensite == "forte")   intensity = Intensity::Strong;
					else return JSONParserStatus::UnknownIntensity;

					stash.stringToIntensities_.emplace(nom_intensite, intensity);
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("casier_depart",
			[&stash](json const & object)
			{
				Pos2D drawerStart; // obviously should check if its a list and if it has size == 2
				drawerStart.x = object.at(0).get<int>();
				drawerStart.y = object.at(1).get<int>();

				stash.casierDepart_ = drawerStart;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("casiers",
			[&stash](json const & object)
			{
				for (auto const & nested : object)
				{
					Drawer drawer;

					drawer.location_.x   = nested.at("colonne").get<int>();
					drawer.location_.y   = nested.at("ligne").get<int>();

					std::string emotionName_ = nested.at("emotion").get<std::string>();
					drawer.emotion_ = stash.stringToEmotions_[emotionName_];
					std::string intensityName_ = nested.at("intensite").get<std::string>();
					drawer.intensity_ = stash.stringToIntensities_[intensityName_];
					drawer.item_ = ((nested.at("objet").is_null()) ? "vide" : nested.at("objet").get<std::string>());

					stash.drawers_.push_back(drawer);
				}

				return JSONParserStatus::Ok;
			}
		)
	};

	result = parser.fillFields(object);
}