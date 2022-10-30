#ifndef ASPELL_HPP
# define ASPELL_HPP

#include <iostream>

#include "ATarget.hpp"

class ATarget;

class ASpell
{
	protected : 
		std::string _name;
		std::string _effects;

	public : 
		ASpell();
		ASpell(std::string const &name, std::string const &effects);
		ASpell(ASpell const & copy);
		ASpell & operator=(ASpell const &assign);
		~ASpell();

		std::string const getName() const;
		std::string const getEffects() const;

		virtual ASpell *clone() = 0;

		void	launch(ATarget const &target) const;
};

#endif