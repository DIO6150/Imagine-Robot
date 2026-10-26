
#include <Robot/Simulation/Dictionary.hpp>
#include <Robot/Parsers/DictionaryParser.hpp>

JSONParserStatus Dictionary::parseJSON(json const & object)
{
	return DictionaryParser {*this, object}.result;
}

std::vector<std::string> normalize(std::string message)
{
	std::vector<std::string> liste_mots;
	std::string mot;
	char cur;
	for(char ch : message)
	{
		if(std::isalpha(ch)) {
			cur = ((std::isupper(ch)) ? ch + 32 : ch);
			mot.push_back(cur);
		} 
		else
		{
			liste_mots.push_back(mot);
			mot.clear();
		}
	}
	return liste_mots;
}

std::tuple<Emotion, Intensity> Dictionary::identifEmoIntens(std::string message)
{
	std::vector<std::string> liste_mots = normalize(message);
	std::tuple<Emotion, Intensity> EmoIntens;
	std::get<0>(EmoIntens) = Emotion::Unidentified;
	std::get<1>(EmoIntens) = Intensity::Unidentified;
	for(std::string mot : liste_mots) {
		if (entrees_.contains(mot))
		{
			Entree entree = entrees_.at(mot);
			//std::cout << mot << " : " << emotionToInt(entree.emotion_) << "," << intensityToInt(entree.intensity_) << std::endl;
			std::get<0>(EmoIntens) = entree.emotion_;
			std::get<1>(EmoIntens) = entree.intensity_;
			//std::cout << emotionToInt(std::get<0>(EmoIntens)) << "," << intensityToInt(std::get<1>(EmoIntens)) << std::endl;
			return EmoIntens;
		}
	}
	return EmoIntens;
}

void Dictionary::dictionaryInfos()
{
	std::cout << "Nom dictionnaire : " << name_ << std::endl;
	for (auto & entree : entrees_)
	{
		std::cout << "Mot : " << entree.first << ", emotion : " << emotionToInt(entree.second.emotion_);
		std::cout << ", intensite" << intensityToInt(entree.second.intensity_) << std::endl;
	}
}
