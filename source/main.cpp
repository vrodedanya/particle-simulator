#include <SDL2/SDL.h>
#include <iostream>
#include "dbhelper.h"
#include "CLI11.hpp"
#include <thread>
#include "implementation/waterfall.h"

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
	CLI::App app{"Waterfall - funny waterfall simulator"};

	unsigned drop_number = 0;
	unsigned stones_size = 0;
	bool isFullscreen = false;
	unsigned window_width = 0;
	unsigned window_height = 0;
	app.add_option("-d,--dropNumber", drop_number, "Number of drops")
		->required(true);
	app.add_option("-s,--stonesSize", stones_size, "Stones' size")
		->required(true);
	app.add_flag("-f,--isFullscreen", isFullscreen, "Is window fullscreen mode?");
	app.add_option("--width", window_width, "Window's width");
	app.add_option("--height", window_height, "Window's height");
		
	CLI11_PARSE(app, argc, argv);

	if (stones_size == 0 || drop_number == 0)
	{
		return 1;
	}

	srand(time(NULL));

	SDL_Init(SDL_INIT_EVERYTHING);

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

	SDL_Window* window = SDL_CreateWindow("Waterfall", 0, 0, window_width, window_height, flag);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	Manager* wf;
	wf = new Waterfall(renderer, drop_number, stones_size, window_width, window_height);

	bool isWork = true;

	SDL_Event event;

	SDL_ShowCursor(0);

	std::thread handler(event_handler, std::ref(event), std::ref(isWork));
	while (isWork)
	{
#ifdef DEBUG
		std::cout << "Begin" << std::endl;
#endif 
		DBHelper::begin();
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
// Draw here
		wf->update();
		wf->draw();

		SDL_RenderPresent(renderer);
		DBHelper::end();
#ifdef DEBUG
		std::cout << "End" << std::endl;
#endif
	}
	delete wf;
	handler.join();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
