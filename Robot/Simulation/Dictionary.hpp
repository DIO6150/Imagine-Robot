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

	friend class DictionaryParser;
};
