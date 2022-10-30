#include "Polymorph.hpp"

Polymorph::Polymorph()
{
	_name = "Polymorph";
	_effects = "turned int a critter";
}

Polymorph::Polymorph(Polymorph const &copy)
{
	_name = copy._name;
	_effects = copy._effects;
}

Polymorph const &Polymorph::operator=(Polymorph const &assign)
{
	if (this != &assign)
	{
		_name = assign._name;
		_effects = assign._name;
	}
	return (*this);
}

Polymorph::~Polymorph(){}

Polymorph *Polymorph::clone()
{
	return (new Polymorph());
}