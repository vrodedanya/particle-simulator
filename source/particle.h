#ifndef PARTICLE_H
#define PARTICLE_H

#include <SDL2/SDL.h>

class Particle
{
public:
	virtual ~Particle(){};
	virtual void draw(SDL_Renderer* renderer){};
	virtual void update(){};
private:
	double xpos;
	double ypos;
};

#endif
