#include "ATarget.hpp"

ATarget::ATarget(){}

ATarget::ATarget(ATarget const &copy)
{
	this->_type = copy._type;
}

ATarget const & ATarget::operator=(ATarget const &assign)
{
	if (this != &assign)
		this->_type = assign._type;
	return (*this);
}

ATarget::~ATarget(){}

std::string const &ATarget::getType() const
{
	return (this->_type);
}

void	ATarget::getHitBySpell(ASpell const &spell) const
{
	std::cout << this->getType() << " has been " << spell.getEffects() << "!\n";
	
}