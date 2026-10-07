#pragma once

#include <stdint.h>

#include <string>
#include <map>
#include <vector>

#include <Robot/Common/Emotion.hpp>

#include <Robot/Errors/Error.hpp>

#include <Robot/Utils/JSONParser.hpp>

#include <vendor/nlohmann/json.hpp>

struct Entree
{
	std::vector<std::string> formes_;
    Emotion emotion_;
    Intensity intensity_;
};

class Dictionary
{
public:
	JSONParserStatus parseJSON(json const & object);

private:
	std::string name_;
	std::vector<Entree> entrees_;
	
	std::string const FORMAT_  = "robot-reconfort/dictionnaire";
	int         const VERSION_ = 1;
    
	std::map<std::string, Emotion> stringToEmotions_;
	std::map<std::string, Intensity> stringToIntensities_;

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

					if (nom_emotion == "joie")              emotion = Emotion::Joy;
					else if (nom_emotion == "confiance")    emotion = Emotion::Confidence;
					else if (nom_emotion == "peur")         emotion = Emotion::Fear;
					else if (nom_emotion == "surprise")     emotion = Emotion::Surprised;
					else if (nom_emotion == "tristesse")    emotion = Emotion::Sadness;
					else if (nom_emotion == "degout")       emotion = Emotion::Disgust;
					else if (nom_emotion == "colere")       emotion = Emotion::Anger;
					else if (nom_emotion == "anticipation") emotion = Emotion::Anticipation;
					else return JSONParserStatus::UnknownEmotion;

					stringToEmotions_.emplace(nom_emotion, emotion);
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

					if (nom_intensite == "faible")       intensity = Intensity::Small;
					else if (nom_intensite == "moyenne") intensity = Intensity::Mid;
					else if (nom_intensite == "forte")   intensity = Intensity::Strong;
					else return JSONParserStatus::UnknownIntensity;

					stringToIntensities_.emplace(nom_intensite, intensity);
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("entrees",
			[this](json const & object)
			{
				for (auto const & nested : object)
				{
					Entree entree;

                    for (auto const & form : nested.at("formes"))
                    {
                        entree.formes_.push_back(form.get<std::string>());
                    }

					std::string emotionName_ = nested.at("emotion").get<std::string>();
					entree.emotion_ = stringToEmotions_[emotionName_];
					std::string intensityName_ = nested.at("intensite").get<std::string>();
					entree.intensity_ = stringToIntensities_[intensityName_];

					entrees_.push_back(entree);
				}

				return JSONParserStatus::Ok;
			}
		)
	};
};
