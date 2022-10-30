#include "BrickWall.hpp"

BrickWall::BrickWall()
{
	_type = "Inconspicuous Red-brick Wall";
}

BrickWall::BrickWall(BrickWall const &copy)
{
	_type = copy._type;
}

BrickWall const &BrickWall::operator=(BrickWall const & assign)
{
	if (this != &assign)
		_type = assign._type;
	return (*this);
}

BrickWall::~BrickWall(){}

BrickWall	*BrickWall::clone()
{
	return (new BrickWall());
}