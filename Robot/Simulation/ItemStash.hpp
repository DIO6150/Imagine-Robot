#pragma once

#include <Robot/Utils/JSONParser.hpp>

#include <Robot/Common/Item.hpp>

struct Drawer
{
	Pos2D location_;
	std::string emotionName_;
	std::string intensityName_;
	std::string item_;
};

class ItemStash
{
public:
	JSONParserStatus parseJSON(json const & object);

	std::string getItem(Pos2D pos);

private:
	std::string name_;
	std::vector<Drawer> drawers_;
	
	std::string const FORMAT_  = "robot-reconfort/armoire";
	int         const VERSION_ = 1;
	Pos2D casierDepart_;
	std::map<std::string, Emotion> emotions_;
	std::map<std::string, Intensity> intensities_;

	JSONParser parser_ {
		Field::validate("format",
			[this](json const & object) { return object.get<std::string>() == FORMAT_; }
		),
		Field::validate("version",
			[this](json const & object) { return object.get<int>() == VERSION_; }
		),
		Field::fillNoError("nom",
			[this](json const & object) { name_ = object.get<std::string>(); }
		),
		Field::fill("emotions",
			[this](json const & object)
			{
				for (auto const & nested : object)
				{
					auto nom_emotion = nested.get<std::string>();
					Emotion emotion;

					if (nom_emotion == "joie")              emotion = Joy;
					else if (nom_emotion == "confiance")    emotion = Confidence;
					else if (nom_emotion == "peur")         emotion = Fear;
					else if (nom_emotion == "surprise")     emotion = Surprised;
					else if (nom_emotion == "tristesse")    emotion = Sadness;
					else if (nom_emotion == "degout")       emotion = Disgust;
					else if (nom_emotion == "colere")       emotion = Anger;
					else if (nom_emotion == "anticipation") emotion = Anticipation;
					else return JSONParserStatus::UnknownEmotion;

					emotions_.emplace(nom_emotion, emotion);
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("intensites",
			[this](json const & object)
			{
				for (auto const & nested : object)
				{
					auto nom_intensite = nested.get<std::string>();
					Intensity intensity;

					if (nom_intensite == "faible")       intensity = Small;
					else if (nom_intensite == "moyenne") intensity = Mid;
					else if (nom_intensite == "forte")   intensity = Strong;
					else return JSONParserStatus::UnknownIntensity;

					intensities_.emplace(nom_intensite, intensity);
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("casier_depart",
			[this](json const & object)
			{
				Pos2D drawerStart; // obviously should check if its a list and if it has size == 2
				drawerStart.x = object.at(0).get<int>();
				drawerStart.y = object.at(1).get<int>();

				auto tile  = getDrawer(drawerStart);

				casierDepart_ = drawerStart;

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("casiers",
			[this](json const & object)
			{
				for (auto const & nested : object)
				{
					Drawer drawer;

					drawer.location_.x   = nested.at("colonne").get<int>();
					drawer.location_.y   = nested.at("ligne").get<int>();

					drawer.emotionName_ = nested.at("emotion").get<std::string>();
					drawer.intensityName_ = nested.at("intensite").get<std::string>();
					drawer.item_ = nested.at("objet").get<std::string>();

					drawers_.push_back(drawer);
				}

				return JSONParserStatus::Ok;
			}
		)
	};
};
