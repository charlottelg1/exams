#ifndef WARLOCK_HPP
# define WARLOCK_HPP

# include <iostream>

class Warlock
{
	private :
		std::string	_name;
		std:: string _title;

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
};

#endif