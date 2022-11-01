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

TargetGenerator::~TargetGenerator()
{
	std::map<std::string, ATarget*>::iterator it = _generator.begin();
	for (; it != _generator.end();++it)
		delete it->second;
	_generator.clear();
}

void TargetGenerator::learnTargetType(ATarget*target)
{
	if (target)
		_generator.insert(std::pair<std::string, ATarget*>(target->getType(), target->clone()));
}

void TargetGenerator::forgetTargetType(std::string const &target)
{
	std::map<std::string, ATarget*>::iterator it = _generator.find(target);
	if (it != _generator.end())
		delete it->second;
	_generator.erase(target);
}

ATarget* TargetGenerator::createTarget(std::string const &target)
{
	std::map<std::string, ATarget*>::iterator it = _generator.find(target);
	if (it != _generator.end())
		return (_generator[target]);
	return (NULL);
}