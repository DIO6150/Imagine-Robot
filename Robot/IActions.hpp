#include <string>

class IAction
{
	virtual ~IAction() = 0;

	virtual std::string getName() = 0;

	public:
		void avancer(int dir);

		int consulter();
		
		int chercher(int dir);
		
		void prendre(string nomObj);
		
		void donner(string nomObj);
		
		void attendre();
};