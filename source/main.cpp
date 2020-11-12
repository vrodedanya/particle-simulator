#include <SDL2/SDL.h>
#include <iostream>
#include "dbhelper.h"
#include "CLI11.hpp"
#include <thread>
#include "implementation/waterfall.h"
#include "implementation/fire.h"

void event_handler(SDL_Event& event, bool& isWork)
{
	while (1)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) 
			{
				isWork = false;
				return;
			}
		}
	}
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	srand(time(NULL));

// Parse arguments
	CLI::App app{"ParticleSimulator"};

	unsigned particle_number = 0;
	unsigned stones_size = 0;
	bool isFullscreen = false;
	unsigned window_width = 0;
	unsigned window_height = 0;
	std::string simulator{""};
	app.add_option("--simulator", simulator, "Choose simulator (waterfall/fire)")
		->required(true);
	app.add_option("-p,--particleNumber", particle_number, "Number of particles")
		->required(true);
	app.add_option("-s,--stonesSize", stones_size, "Stones' size for waterfall");
	app.add_flag("-f,--isFullscreen", isFullscreen, "Is window fullscreen mode?");
	app.add_option("--width", window_width, "Window's width");
	app.add_option("--height", window_height, "Window's height");
		
	CLI11_PARSE(app, argc, argv);

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
	SDL_Event event;
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
		manager = new FireManager(renderer, window_width, window_height, particle_number, event.motion.x, event.motion.y, window_width / 2, window_height, window_width / static_cast<double>(1 + 16000 / particle_number), window_height / 9, 0);
	}
	else
	{
		std::cerr << simulator << " simulator doesn't exist" << std::endl;
		return 1;
	}

	bool isWork = true;

	std::thread handler(event_handler, std::ref(event), std::ref(isWork));
	while (isWork)
	{
		DBHelper::begin();
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
// Draw here
		manager->update();
		manager->draw();

		SDL_RenderPresent(renderer);
		DBHelper::end();
	}
	delete manager;
	handler.join();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
