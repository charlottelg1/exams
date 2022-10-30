#include "TargetGenerator.hpp"

TargetGenerator::TargetGenerator(TargetGenerator const &copy)
{
	(void)copy;
}

TargetGenerator const &TargetGenerator::operator=(TargetGenerator const &assign)
{
	(void)assign;
	return (*this);
}

TargetGenerator::TargetGenerator(){}

TargetGenerator::~TargetGenerator(){}

void TargetGenerator::learnTargetType(ATarget*target)
{
	_generator.push_back(target);
}

void TargetGenerator::forgetTargetType(std::string const &target)
{
	std::vector<ATarget*>::iterator it = _generator.begin();

	for (;it != _generator.end(); it++)
	{
		if ((*it)->getType() == target)
		{
			delete *it;
			this->_generator.erase(it);
			return ;
		}	
	}
}

ATarget* TargetGenerator::createTarget(std::string const &target)
{
	size_t i = 0;
	for (; i < _generator.size(); i++)
	{
		if (_generator[i]->getType() == target)
			return (_generator[i]);
	}
	return (0);
}