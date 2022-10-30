#include "Dummy.hpp"

Dummy::Dummy()
{
	this->_type = "Target Practice Dummy";
}

Dummy::Dummy(Dummy const &copy)
{
	this->_type = copy._type;
}

Dummy const &Dummy::operator=(Dummy const &assign)
{
	if (this != &assign)
	{
		_type = assign._type;
	}
	return (*this);
}

Dummy::~Dummy(){}

Dummy *Dummy::clone()
{
	return (new Dummy());
}