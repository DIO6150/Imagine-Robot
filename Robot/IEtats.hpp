#include <string>
#include <vector>
#include <tuple>

class IEtat
{
	virtual ~IEtat() = 0;

	virtual std::string getName() = 0;

	private:
        vector<string> carteMentale;

        struct Resident{
            string identifiant;
            string nom;
            tuple<int,int> position;
        };

        struct Requete{
            struct Resident;
            string message;
            tuple<int,int> emotion;
        };
    
    public:
        void miseAJourCarte(tuple<int, int> position);
        
        int mainVide();
};
