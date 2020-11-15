#ifndef MANAGER_H
#define MANAGER_H

#include <SDL2/SDL_events.h>

class Manager
{
private:
public:
	virtual ~Manager(){}
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void event_handler(SDL_Event& event) = 0;
};

#endif
