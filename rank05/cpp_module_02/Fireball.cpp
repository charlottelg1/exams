#include "Fireball.hpp"

Fireball::Fireball()
{
	_name = "Fireball";
	_effects = "burnt to a crisp";
}

Fireball::Fireball(Fireball const &copy)
{
	_name = copy._name;
	_effects = copy._effects;
}

Fireball const &Fireball::operator=(Fireball const &assign)
{
	if (this != &assign)
	{
		_name = assign._name;
		_effects = assign._effects;
	}
	return (*this);
}

Fireball::~Fireball(){}

Fireball *Fireball::clone()
{
	return (new Fireball());
}