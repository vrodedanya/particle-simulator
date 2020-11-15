#ifndef MANAGER_H
#define MANAGER_H

#include <SDL2/SDL_events.h>

class Manager
{
private:
public:
	virtual ~Manager(){};
	virtual void update(){};
	virtual void update_range(int begin, int end){};
	virtual void draw(){};
	virtual void event_handler(SDL_Event& event){}
};

#endif
