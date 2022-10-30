#ifndef BRICKWALL_HPP
# define BRICKWALL_HPP

#include "ATarget.hpp"

class BrickWall : public ATarget
{
	public :
		BrickWall();
		BrickWall(BrickWall const &copy);
		BrickWall const &operator=(BrickWall const & assign);
		~BrickWall();

		BrickWall	*clone();
};

#endif