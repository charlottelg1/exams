#ifndef POLYMORPH_HPP
# define POLYMORPH_HPP

#include "ASpell.hpp"

class Polymorph : public ASpell
{
	public :
		Polymorph();
		Polymorph(Polymorph const &copy);
		Polymorph const &operator=(Polymorph const &assign);
		~Polymorph();

		Polymorph *clone();
};

#endif