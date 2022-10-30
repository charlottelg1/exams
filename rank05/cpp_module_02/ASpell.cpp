#include "ASpell.hpp"

ASpell::ASpell(){};

ASpell::ASpell(std::string const &name, std::string const &effects)
{
	this->_name = name;
	this->_effects = effects;
}

ASpell::ASpell(ASpell const & copy)
{
	this->_name = copy._name;
	this->_effects = copy._effects;
}
		
ASpell & ASpell::operator=(ASpell const &assign)
{
	if (this != &assign)
	{
		this->_name = assign._name;
		this->_effects = assign._effects;
	}
	return (*this);
}

ASpell::~ASpell(){}

std::string const ASpell::getName() const
{
	return (this->_name);
}

std::string const ASpell::getEffects() const
{
	return (this->_effects);
}

void	ASpell::launch(ATarget const &target) const
{
	target.getHitBySpell((*this));
}

// ASpell *ASpell::clone(){}