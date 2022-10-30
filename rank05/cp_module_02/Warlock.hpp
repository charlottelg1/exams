#ifndef WARLOCK_HPP
# define WARLOCK_HPP

# include <iostream>
# include "ASpell.hpp"
# include "ATarget.hpp"
# include "SpellBook.hpp"
# include <map>

class Warlock
{
	private :
		std::string	_name;
		std:: string _title;

		SpellBook	_spellBook;

		Warlock();
		Warlock(Warlock & copy);
		Warlock & operator=(Warlock const & assign);

	public :
		Warlock(std::string const &name, std::string const &title);
		~Warlock();
		
		std::string const & getName() const;
		std::string const & getTitle() const;

		void	setTitle(std::string const & str);	
		
		void	introduce() const;

		void	learnSpell(ASpell *newSpell);
		void	forgetSpell(std::string spellName);
		void	launchSpell(std::string spellName, ATarget &target);
};

#endif