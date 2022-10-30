#ifndef TARGETGENERATOR_HPP
# define TARGETGENERATOR_HPP

# include "ATarget.hpp"
#include <vector>

class TargetGenerator
{
	private : 
		TargetGenerator(TargetGenerator const &copy);
		TargetGenerator const &operator=(TargetGenerator const &assign);

		std::vector<ATarget *>	_generator;

	public :
		TargetGenerator();
		~TargetGenerator();

		void learnTargetType(ATarget*target);
		void forgetTargetType(std::string const &target);
		ATarget* createTarget(std::string const &target);
};

#endif