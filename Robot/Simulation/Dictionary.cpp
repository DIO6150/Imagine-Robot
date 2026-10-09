#include <Robot/Simulation/Dictionary.hpp>

JSONParserStatus Dictionary::parseJSON(json const & object)
{
	return parser_.fillFields(object);
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

bool Dictionary::identifEmoIntens(std:tuple<Emotion, Intensity> & EmoIntens, std::string message)
{
	std::vector<std::string> liste_mots = normalize(message);
	for(std::string mot : liste_mots) {
		for(Entree entree : entrees_) {
			if(count(entree.formes_.begin(), entree.formes_.end(), mot) > 0)
			{
				get<0>(EmoIntens) = entree.emotion_;
				get<1>(EmoIntens) = entree.intensity_;
				return true;
			}
		}
	}
	return false;
}