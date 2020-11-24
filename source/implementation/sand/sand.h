#ifndef SAND_H
#define SAND_H

#include <vector>
#include <SDL2/SDL.h>
#include "../../dbhelper.h"
#include "../../manager.h"

class Sand 
{
private:
public:
	SDL_Rect rect;
	double x;
	double y;
	bool isMoved;
	explicit Sand(double xpos, double ypos);
	~Sand();
	void draw(SDL_Renderer* renderer);
	void update(std::vector<Sand*> sandbox, unsigned dwidth, unsigned dheight);
	double fall_speed;
};

class SandManager : public Manager
{
private:
	std::vector<Sand*> sandbox;
	SDL_Renderer* renderer;
	unsigned& dwidth;
	unsigned& dheight;

	bool isPressed;
public:
	explicit SandManager(SDL_Renderer* render, unsigned part_count, unsigned& window_width, unsigned& window_height) : renderer(render), dwidth(window_width), dheight(window_height), isPressed(false)
	{
		for (unsigned i = 0 ; i < part_count ; i++)
		{
			Sand* buf = new Sand(-1, -1);
			sandbox.emplace_back(buf);
		}
	}
	~SandManager()
	{
		for (auto& sand : sandbox)
		{
			delete sand;
		}
	}
	void draw();
	void update();
	void event_handler(SDL_Event& event);
};

#endif
