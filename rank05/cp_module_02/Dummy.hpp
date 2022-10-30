#ifndef DUMMY_HPP
# define DUMMY_HPP

#include "ATarget.hpp"

class Dummy : public ATarget
{
	public : 
		Dummy();
		Dummy(Dummy const &copy);
		Dummy const &operator=(Dummy const &assign);
		~Dummy();

		Dummy *clone();
};

#endif