#include "Fwoosh.hpp"

Fwoosh::Fwoosh()
{
	_name = "Fwoosh";
	_effects = "fwooshed";
}

Fwoosh::Fwoosh(Fwoosh const &copy)
{
	this->_name = copy._name;
	this->_effects =copy._effects;
}

Fwoosh const &Fwoosh::operator=(Fwoosh const &assign)
{
	if (this != &assign)
	{
		this->_name = assign._name;
		this->_effects =assign._effects;
	}
	return (*this);
}

Fwoosh::~Fwoosh(){}

Fwoosh *Fwoosh::clone()
{
	return (new Fwoosh());
}