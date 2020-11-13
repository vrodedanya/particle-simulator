#include "fire.h"

Fire::Fire()
{
	x = -1;
	y = -1;
	environment = 0;
}
Fire::~Fire()
{}

void FireManager::spawn_part(int begin, int end)
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

void FireManager::draw()
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

void FireManager::move(int begin, int end)
{
	double speed;
	for (int i = begin ; i < end ; i++)
	{
		if (particles[i]->t > 0)
		{	
			speed = DBHelper::delta * (30 + particles[i]->t) * (static_cast<int>(dheight) / 120);
			particles[i]->y -= speed;
			if (pow(particles[i]->x - event.motion.x, 2) + pow(particles[i]->y - event.motion.y, 2) <= pow(30, 2))
			{
				if (particles[i]->x > event.motion.x)
				{
					particles[i]->x = sqrt(pow(30, 2) - pow(particles[i]->y - event.motion.y, 2)) + event.motion.x;
				}
				else
				{
					particles[i]->x = -1 * sqrt(pow(30, 2) - pow(particles[i]->y - event.motion.y, 2)) + event.motion.x;
				}
			}
			else
			{
				particles[i]->x = particles[i]->x + (-(dwidth/6.4) + rand()%static_cast<int>(dwidth/3.84)) * DBHelper::delta;
			}
		}
	}
}

void FireManager::check(int begin, int end)
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

void FireManager::zeroize_env(int begin, int end)
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

void FireManager::update()
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

bool FireManager::event_handler()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			return false;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
		{
			return false;
		}
	}
	return true;
}
