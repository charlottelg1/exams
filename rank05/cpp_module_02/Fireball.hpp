#ifndef FIREBALL_HPP
# define FIREBALL_HPP

#include <iostream>

#include "ASpell.hpp"

class Fireball : public ASpell
{
	public :
		Fireball();
		Fireball(Fireball const &copy);
		Fireball const &operator=(Fireball const &assign);
		~Fireball();

		Fireball *clone();
};

#endif
