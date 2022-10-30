#ifndef SPELLBOOK_HPP
# define SPELLBOOK_HPP

#include <iostream>

#include "ASpell.hpp"
#include <map>

class SpellBook
{
	private : 
		std::map<std::string, ASpell*>	_spellBook;
		
		SpellBook(SpellBook const &copy);
		SpellBook const &operator=(SpellBook const &assign);

	public : 
		SpellBook();
		~SpellBook();

		void	learnSpell(ASpell*spell);
		void forgetSpell(std::string const &spell);
		ASpell* createSpell(std::string const &spell);
};

#endif