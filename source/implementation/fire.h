#ifndef FIRE_H
#define FIRE_H

#include "../particle.h"
#include "../manager.h"
#include "../dbhelper.h"
#include <vector>
#include <thread>
#include <SDL2/SDL.h>
#include <iostream>

class Fire : public Particle
{
private:
public:
	Fire();
	~Fire();
	double t;
	double x;
	double y;
	unsigned environment;

};
class FireManager : public Manager
{
private:
	unsigned int threads_count;
	SDL_Renderer* renderer;
	std::vector<Fire*> particles;

	int center_x;
	int center_y;
	int radius_x;
	int radius_y;
	int exclude_radius;
	
	unsigned& dwidth;
	unsigned& dheight;

	int exclude_x;
	int exclude_y;
public:
	explicit FireManager(SDL_Renderer* renderer, unsigned& window_width, unsigned& window_height, unsigned count, int cen_x, int cen_y, int rad_x, int rad_y, int exc_rad) : center_x(cen_x), center_y(cen_y), radius_x(rad_x), radius_y(rad_y), exclude_radius(exc_rad), dwidth(window_width), dheight(window_height)
	{
		this->renderer = renderer;
		std::cout << count << std::endl;
		for (unsigned i = 0 ; i < count ; i++)
		{
			Fire* buf = new Fire;
			buf->t = 0;
			buf->x = 0;
			buf->y = -1;
			buf->environment = 0;
			particles.emplace_back(buf);
		}
		threads_count = std::thread::hardware_concurrency();
	}
	~FireManager()
	{
		for (unsigned i = 0 ; i < particles.size() ; i++)
		{
			delete particles[i];
		}
	}
	void spawn_part(int begin, int end);
	void draw();
	void move(int begin, int end);
	void check(int begin, int end);
	void event_handler(SDL_Event& event);

	void zeroize_env(int begin, int end);
	void update();
};

#endif
