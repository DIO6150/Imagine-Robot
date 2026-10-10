

#include <Robot/Parsers/DictionaryParser.hpp>
#include <Robot/Simulation/Dictionary.hpp>

#include <Robot/Utils/JSONParser.hpp>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

DictionaryParser::DictionaryParser(Dictionary & dictionary, json const & object)
{
	JSONParser parser {
		Field::validate("format",
			[&dictionary](json const & object) { return object.get<std::string>() == dictionary.FORMAT_; }
		),
		Field::validate("version",
			[&dictionary](json const & object) { return object.get<int>() == dictionary.VERSION_; }
		),
		Field::fillNoError("nom",
			[&dictionary](json const & object) { dictionary.name_ = object.get<std::string>(); }
		),
		Field::fill("emotions",
			[&dictionary](json const & object)
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

					dictionary.stringToEmotions_.emplace(nom_emotion, emotion);
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("intensites",
			[&dictionary](json const & object)
			{
				for (auto const & nested : object)
				{
					auto nom_intensite = nested.get<std::string>();
					Intensity intensity;

					if (nom_intensite == "faible")       intensity = Intensity::Small;
					else if (nom_intensite == "moyenne") intensity = Intensity::Mid;
					else if (nom_intensite == "forte")   intensity = Intensity::Strong;
					else return JSONParserStatus::UnknownIntensity;

					dictionary.stringToIntensities_.emplace(nom_intensite, intensity);
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("entrees",
			[&dictionary](json const & object)
			{
				for (auto const & nested : object)
				{
					Entree entree;

                    for (auto const & form : nested.at("formes"))
                    {
                        entree.formes_.push_back(form.get<std::string>());
                    }

					std::string emotionName_ = nested.at("emotion").get<std::string>();
					entree.emotion_ = dictionary.stringToEmotions_[emotionName_];
					std::string intensityName_ = nested.at("intensite").get<std::string>();
					entree.intensity_ = dictionary.stringToIntensities_[intensityName_];

					dictionary.entrees_.push_back(entree);
				}

				return JSONParserStatus::Ok;
			}
		)
	};

	result = parser.fillFields(object);
}