#include <SDL2/SDL.h>
#include <iostream>
#include "dbhelper.h"
#include <thread>
#include "implementation/waterfall.h"
#include "implementation/fire.h"
#include "implementation/net.h"
#include "implementation/sand.h"
#include "clishell.h"

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	srand(time(NULL));

	unsigned particle_number = 0;
	unsigned stones_size = 0;
	bool isFullscreen = false;
	unsigned window_width = 0;
	unsigned window_height = 0;
	std::string simulator{""};

	CAparser(argc, argv, simulator, particle_number, stones_size, isFullscreen, window_width, window_height);

// Set options
	if (particle_number <= 0)
	{
		std::cerr << "Particle number is equal or lower than zero" << std::endl;
		return 1;
	}

	Uint32 flag = SDL_WINDOW_FULLSCREEN;
	if (!isFullscreen)
	{
		flag = SDL_WINDOW_SHOWN;
	}

	SDL_DisplayMode dm = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

	if (SDL_GetDisplayMode(0, 0, &dm) != 0)
	{
		std::cerr << SDL_GetError() << std::endl;
		return 1;
	}
	if (window_height == 0)
	{
		window_height = dm.h;
	}
	if (window_width == 0)
	{
		window_width = dm.w;
	}

// SDL2
	SDL_Window* window = SDL_CreateWindow("ParticleSimulator", 0, 0, window_width, window_height, flag);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	Manager* manager;

	if (simulator == "waterfall")
	{
		if (stones_size == 0)
		{
			std::cerr << "Enter stones size" << std::endl;
			return 1;
		}
		manager = new Waterfall(renderer, particle_number, stones_size, window_width, window_height);
		SDL_ShowCursor(0);
	}
	else if (simulator == "fire")
	{
		manager = new FireManager(renderer, window_width, window_height, particle_number, window_width / 2, window_height, window_width / static_cast<double>(1 + 16000 / particle_number), window_height / 9, 0);
	}
	else if (simulator == "net")
	{
		manager = new NetManager(renderer, particle_number, window_width, window_height);
	}
	else if (simulator == "sandbox")
	{
		manager = new SandManager(renderer, particle_number, window_width, window_height);
	}
	else
	{
		std::cerr << simulator << " simulator doesn't exist" << std::endl;
		return 1;
	}

	bool isWork = true;

	SDL_Event event;

	while (isWork)
	{
		DBHelper::begin();
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
// Draw here
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) isWork = false;
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						isWork = false;
						break;
				}
			}
			manager->event_handler(event);
		}
		manager->update();
		manager->draw();

		SDL_RenderPresent(renderer);
		DBHelper::end();
	}
	delete manager;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
