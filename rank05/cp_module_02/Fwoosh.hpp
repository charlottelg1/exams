#ifndef FWOOSH_HPP
# define FWOOSH_HPP

# include "ASpell.hpp"

class Fwoosh : public ASpell
{
	public :
		Fwoosh();
		Fwoosh(Fwoosh const &copy);
		Fwoosh const & operator=(Fwoosh const &assign);
		virtual ~Fwoosh();

		Fwoosh *clone();
};

#endif