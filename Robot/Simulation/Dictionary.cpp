
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
	for(std::string mot : liste_mots) {
		for(Entree entree : entrees_) {
			if(count(entree.formes_.begin(), entree.formes_.end(), mot) > 0)
			{
				std::get<0>(EmoIntens) = entree.emotion_;
				std::get<1>(EmoIntens) = entree.intensity_;
				return EmoIntens;
			}
		}
	}
	std::get<0>(EmoIntens) = Emotion::Unidentified;
	std::get<1>(EmoIntens) = Intensity::Unidentified;
	return EmoIntens;
}

void Dictionary::dictionaryInfos()
{
	std::cout << "Nom dictionnaire : " << name_ << std::endl;
	for (Entree ent : entrees_)
	{
		std::cout << "Entrees : ";
		for (std::string form : ent.formes_)
			std::cout << form;
		std::cout << "." << std::endl;
	}
}
