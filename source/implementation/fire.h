#ifndef FIRE_H
#define FIRE_H

#include "../particle.h"
#include "../manager.h"
#include <vector>
#include <thread>
#include <SDL2/SDL.h>

class Fire : public Particle
{
private:
public:
	Fire()
	{
		x = -1;
		y = -1;
		environment = 0;
	}
	~Fire()
	{
	}
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
	int& exclude_x;
	int& exclude_y;

	int center_x;
	int center_y;
	int radius_x;
	int radius_y;
	int exclude_radius;
	
	unsigned& dwidth;
	unsigned& dheight;
public:
	explicit FireManager(SDL_Renderer* renderer, unsigned& window_width, unsigned& window_height, unsigned count, int& exc_x, int& exc_y, int cen_x, int cen_y, int rad_x, int rad_y, int exc_rad) : exclude_x(exc_x), exclude_y(exc_y), center_x(cen_x), center_y(cen_y), radius_x(rad_x), radius_y(rad_y), exclude_radius(exc_rad), dwidth(window_width), dheight(window_height)
	{
		this->renderer = renderer;
		for (unsigned i = 0 ; i < count ; i++)
		{
			Fire* buf = new Fire;
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
	void spawn_part(int begin, int end)
	{
		int radius_buffer = 0;
		double angle;
		for (int i = begin ; i < end ; i++)
		{
			if (particles[i]->y <= 0 || particles[i]->t <= 0)
			{
				angle = rand()%365 * 0.0174;
				if (radius_x != 0) radius_buffer = exclude_radius + rand()%radius_x;
				particles[i]->x = center_x + cos(angle) * radius_buffer;
				if (radius_y != 0) radius_buffer = exclude_radius + rand()%radius_y;
				particles[i]->y = center_y + sin(angle) * radius_buffer;

				particles[i]->t = 20;
			}
			else continue;
		}
	}
	void draw() override
	{
		Uint8 green;
		for (auto& particle : particles)
		{
			if (particle->t > 0)
			{
				green =  pow(particle->t, 2);
				if (green > 255) green = 255;
				SDL_SetRenderDrawColor(renderer, 255, green, 0, 0);
				SDL_RenderDrawPoint(renderer, particle->x, particle->y);
			}
		}
	}
	void move(int begin, int end)
	{
		double speed;
		for (int i = begin ; i < end ; i++)
		{
			if (particles[i]->t > 0)
			{	
				speed = DBHelper::delta * (30 + particles[i]->t) * (static_cast<int>(dheight) / 120);
				particles[i]->y -= speed;
				if (pow(particles[i]->x - exclude_x, 2) + pow(particles[i]->y - exclude_y, 2) <= pow(30, 2))
				{
					if (particles[i]->x > exclude_x)
					{
						particles[i]->x = sqrt(pow(30, 2) - pow(particles[i]->y - exclude_y, 2)) + exclude_x;
					}
					else
					{
						particles[i]->x = -1 * sqrt(pow(30, 2) - pow(particles[i]->y - exclude_y, 2)) + exclude_x;
					}
				}
				else
				{
					particles[i]->x = particles[i]->x + (-(1920/6.4) + rand()%static_cast<int>(1920/3.84)) * DBHelper::delta;
				}
			}
		}
	}
	void check(int begin, int end)
	{
		for (int i = begin ; i < end ; i++)
		{
			for (std::vector<Particle*>::size_type j = i + 1 ; j < particles.size() ; j++)
			{
				if (sqrt(pow(particles[i]->x - particles[j]->x, 2) + pow(particles[i]->y - particles[j]->y, 2)) <= 10)
				{
					particles[i]->environment++;
					particles[j]->environment++;
				}
			}
		}
	}

	void zeroize_env(int begin, int end)
	{
		for (int i = begin ; i < end ; i++)
		{
			if (particles[i]->environment >= 10)
			{	
				particles[i]->t++;
			}
			else if (particles[i]->environment == 0)
			{
				particles[i]->t-=8;
			}
			particles[i]->environment = 0;
		}
	}
	void update()
	{
		std::thread threads[threads_count];
		for (unsigned i = 0 ; i < threads_count ; i++)	
		{
			threads[i] = std::thread(&FireManager::spawn_part, this, particles.size() / threads_count * i, particles.size() / threads_count * (i + 1));
		}
		for (unsigned i = 0 ; i < threads_count ; i++)
		{
			threads[i].join();
		}
		for (unsigned i = 0 ; i < threads_count ; i++)
		{
			threads[i] = std::thread(&FireManager::check, this, i * particles.size() / threads_count, particles.size() / threads_count * (i + 1));
		}
		for (unsigned i = 0 ; i < threads_count ; i++)
		{
			threads[i].join();
		}
		for (unsigned i = 0 ; i < threads_count ; i++)
		{
			threads[i] = std::thread(&FireManager::zeroize_env, this, i * particles.size() / threads_count, particles.size() / threads_count * (i + 1));
		}
		for (unsigned i = 0 ; i < threads_count ; i++)
		{
			threads[i].join();
		}
		for (unsigned i = 0 ; i < threads_count ; i++)
		{
			threads[i] = std::thread(&FireManager::move, this, i * particles.size() / threads_count, particles.size() / threads_count * (i + 1));
		}
		for (unsigned i = 0 ; i < threads_count ; i++)
		{
			threads[i].join();
		}
	}
};

#endif
