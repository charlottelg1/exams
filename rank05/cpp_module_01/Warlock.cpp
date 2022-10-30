#include "Warlock.hpp"

Warlock::Warlock(){}

Warlock::Warlock(Warlock & copy){(void)copy;}

Warlock & Warlock::operator=(Warlock const & assign){(void)assign; return (*this);}

Warlock::Warlock(std::string const &name, std::string const &title)
{
	this->_name = name;
	this->_title = title;
	std::cout << _name << ": This looks like another boring day.\n";
}

Warlock::~Warlock()
{
	std::cout << _name << ": My job here is done!\n";
}
		
std::string const & Warlock::getName() const
{
	return (this->_name);
}

std::string const & Warlock::getTitle()const
{
	return (this->_title);
}

void	Warlock::setTitle(std::string const & str)
{
	this->_title = str;
}	

void	Warlock::introduce() const
{
	std::cout << _name <<": I am " << _name << ", " << _title <<"!\n";
}

void	Warlock::learnSpell(ASpell *newSpell)
{
	if (newSpell)
		this->_spell.insert(std::pair<std::string,ASpell*>(newSpell->getName(), newSpell));
}

void	Warlock::forgetSpell(std::string spellName)
{
	this->_spell.erase(this->_spell.find(spellName));
}

void	Warlock::launchSpell(std::string spellName, ATarget &target)
{
	ASpell *spell = this->_spell[spellName];
	if (spell)
		spell->launch(target);
}

// int main()
// {
//   Warlock const richard("Richard", "Mistress of Magma");
//   richard.introduce();
//   std::cout << richard.getName() << " - " << richard.getTitle() << std::endl;

//   Warlock* jack = new Warlock("Jack", "the Long");
//   jack->introduce();
//   jack->setTitle("the Mighty");
//   jack->introduce();

//   delete jack;

//   return (0);
// }