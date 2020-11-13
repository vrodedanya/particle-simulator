#ifndef SAND_H
#define SAND_H

#include <vector>
#include "../dbhelper.h"
#include "../particle.h"
#include "../manager.h"

class Sand : public Particle
{
private:
public:
	double x;
	double y;
	explicit Sand(double xpos, double ypos);
	~Sand();
	void draw(SDL_Renderer* renderer);
	void update();
	double fall_speed;
};

class SandManager : public Manager
{
private:
	std::vector<Sand*> sandbox;
	SDL_Renderer* renderer;
	unsigned& dwidth;
	unsigned& dheight;

	SDL_Event event;
public:
	explicit SandManager(SDL_Renderer* render, unsigned part_count, unsigned& window_width, unsigned& window_height) : renderer(render), dwidth(window_width), dheight(window_height)
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
	bool event_handler();
};

#endif
