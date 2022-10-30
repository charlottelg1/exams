#ifndef ATARGET_HPP
# define ATARGET_HPP

#include "ASpell.hpp"

class ASpell;

class ATarget
{
	protected : 
		std::string	_type;

	public : 
		ATarget();
		ATarget(ATarget const &copy);
		ATarget const & operator=(ATarget const &assign);
		~ATarget();

		std::string const &getType() const;

		virtual ATarget	*clone() = 0;

		void	getHitBySpell(ASpell const &spell) const;
};

#endif