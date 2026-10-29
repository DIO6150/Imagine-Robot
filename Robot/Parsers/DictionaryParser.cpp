

#include <Robot/Parsers/DictionaryParser.hpp>

using json = nlohmann::json;

DictionaryParser::DictionaryParser(Dictionary & dictionary, json const & object)
{
	JSONParser parser {
		{"format", "version", "nom", "emotions", "intensites", "entrees"},
		{
		Field::validate("format",
			[&dictionary](json const & object) 
			{ 
				if (!object.is_string())
					return JSONParserStatus::WrongType;
				if (object.get<std::string>() != dictionary.FORMAT_)
					return JSONParserStatus::WrongFormat;
				return JSONParserStatus::Ok;
			}
		),
		Field::validate("version",
			[&dictionary](json const & object) 
			{ 
				if (!object.is_number_integer())
					return JSONParserStatus::WrongType;
				if (object.get<int>() != dictionary.VERSION_)
					return JSONParserStatus::WrongVersion;
				return JSONParserStatus::Ok;
			}
		),
		Field::validate("nom", JSONParser::verifySimpleString),
		Field::fillNoError("nom",
			[&dictionary](json const & object) { dictionary.name_ = object.get<std::string>(); }
		),
		Field::validate("emotions",
			[&dictionary](json const & object) 
			{ 
				if (!object.is_array())
					return JSONParserStatus::WrongType;

				if (object.size() != emotionToString.size())
					return JSONParserStatus::ArrayEmotions;

				for (auto const & nested : object)
				{
					if (!nested.is_string())
						return JSONParserStatus::WrongType;
					if (std::count(emotionToString.begin(), emotionToString.end(), nested.get<std::string>()) == 0)
						return JSONParserStatus::UnknownEmotion;
				}
				return JSONParserStatus::Ok;
			}
		),
		Field::fillNoError("emotions",
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

					dictionary.stringToEmotions_.emplace(nom_emotion, emotion);
				}
			}
		),
		Field::validate("intensites",
			[&dictionary](json const & object) 
			{ 
				if (!object.is_array())
					return JSONParserStatus::WrongType;

				if (object.size() != intensityToString.size())
					return JSONParserStatus::ArrayIntensity;

				for (auto const & nested : object)
				{
					if (!nested.is_string())
						return JSONParserStatus::WrongType;
					if (std::count(intensityToString.begin(), intensityToString.end(), nested.get<std::string>()) == 0)
						return JSONParserStatus::UnknownIntensity;
				}
				return JSONParserStatus::Ok;
			}
		),
		Field::fillNoError("intensites",
			[&dictionary](json const & object)
			{
				for (auto const & nested : object)
				{
					auto nom_intensite = nested.get<std::string>();
					Intensity intensity;

					if (nom_intensite == "faible")       intensity = Intensity::Small;
					if (nom_intensite == "moyenne") intensity = Intensity::Mid;
					if (nom_intensite == "forte")   intensity = Intensity::Strong;

					dictionary.stringToIntensities_.emplace(nom_intensite, intensity);
				}
			}
		),
		Field::branch("entrees",
			{
				Field::validate("formes",
					[&dictionary](json const & object) 
					{ 
						if (!object.is_array() || object.empty())
							return JSONParserStatus::WrongType;

						for (auto const & form : object)
						{
							if(!form.is_string())
								return JSONParserStatus::WrongType;
						}

						return JSONParserStatus::Ok;
					}
				),
				Field::validate("emotion",
					[&dictionary](json const & object) 
					{ 
						if (!object.is_string())
							return JSONParserStatus::WrongType;

						if (std::count(emotionToString.begin(), emotionToString.end(), object.get<std::string>()) == 0)
								return JSONParserStatus::UnknownEmotion;

						return JSONParserStatus::Ok;
					}
				),
				Field::validate("intensite",
					[&dictionary](json const & object) 
					{ 
						if (!object.is_string())
							return JSONParserStatus::WrongType;

						if (std::count(intensityToString.begin(), intensityToString.end(), object.get<std::string>()) == 0)
								return JSONParserStatus::UnknownIntensity;
							
						return JSONParserStatus::Ok;
					}
				)
			}
		),
		Field::validate("entrees",
			[&dictionary](json const & object)
			{
				if(!object.is_array() || object.empty())
					return JSONParserStatus::WrongType;
				
				for (auto const & nested : object)
				{
					if(!nested.is_object())
						return JSONParserStatus::WrongType;

					if(!nested.contains("formes") || !nested.contains("emotion") || !nested.contains("intensite"))
						return JSONParserStatus::MissingField;
				}

				return JSONParserStatus::Ok;
			}
		),
		Field::fill("entrees",
			[&dictionary](json const & object)
			{
				dictionary.entrees_.clear();
				for (auto const & nested : object)
				{
					Entree entree;

					std::string emotionName_ = nested.at("emotion").get<std::string>();
					entree.emotion_ = dictionary.stringToEmotions_[emotionName_];
					std::string intensityName_ = nested.at("intensite").get<std::string>();
					entree.intensity_ = dictionary.stringToIntensities_[intensityName_];

                    for (auto const & form : nested.at("formes"))
                    {
						std::string mot = form.get<std::string>();

						if (dictionary.entrees_.contains(mot))
							return JSONParserStatus::WordTwiceDefined;
						dictionary.entrees_.emplace(mot, entree);
                    }
				}

				return JSONParserStatus::Ok;
			}
		)
	}
	};

	result = parser.fillFields(object);
}