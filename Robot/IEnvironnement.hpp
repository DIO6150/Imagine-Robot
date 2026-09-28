
#include <string>
#include <vector>

class IEnvironnement
{
	virtual ~IEnvironnement() = 0;

	virtual std::string getName() = 0;

	private:
        vector<string> carte;

        vector<string> dictionnaire;
    
    public:
        int armoire[8][3];
};
