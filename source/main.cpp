#include <SDL2/SDL.h>
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include "dbhelper.h"
#include "CLI11.hpp"

#define WINDOW_W 1920
#define WINDOW_H 1080

class Stone
{
private:
public:
	Stone(unsigned max_stone_size = 300)
	{
		randomize(max_stone_size);
	}
	void randomize(unsigned max_stone_size)
	{
		x = rand()%WINDOW_W;
		y = rand()%WINDOW_H;
		radius = rand()%max_stone_size;
	}
	int x;
	int y;
	int radius;

	int check_isEnter(int xpos, int ypos)
	{
		if (pow(xpos - x, 2) + pow(ypos - y, 2) <= pow(radius, 2))
		{
			int new_x;
			if (xpos > x) new_x = sqrt(pow(radius, 2) - pow(y - ypos, 2)) + x;
			else 		  new_x = -sqrt(pow(radius, 2) - pow(y - ypos, 2)) + x;
			return new_x;
		}
		return xpos;
	}
	void draw(SDL_Renderer* renderer)
	{
		for (int i = 0 ; i < 2 * M_PI * radius ; i++)
		{
			int x_buf = x + cos(i) * radius;
			int y_buf = y + sin(i) * radius;
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
			SDL_RenderDrawPoint(renderer, x_buf, y_buf);
		}
	}
};

class Drop
{
private:
	double xpos;
	double ypos;
	double fall_speed;	
	SDL_Color color;
	bool isBlue;
	std::chrono::system_clock::time_point start;
	void randomize()
	{
		std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
		if (diff.count() >= 10)
		{
			start = std::chrono::system_clock::now();
			isBlue = !isBlue;
		}
		if (isBlue)
		{
			color.r = 170 + rand()%85;
			color.g = 150 + rand()%105;
			color.b = rand()%100;
		}
		else
		{
			color.r = 200 +rand()%55;
			color.g = rand()%100;
			color.b = rand()%100;
		}
		xpos = rand()%WINDOW_W;
		ypos = -(rand()%WINDOW_H * 2);
		fall_speed = rand()%20;
	}
public:
	Drop()
	{
		start = std::chrono::system_clock::now();
		isBlue = true;
		randomize();
	}
	~Drop()
	{
	}

	void draw(SDL_Renderer* renderer)
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0);
		SDL_RenderDrawPoint(renderer, static_cast<int>(xpos), static_cast<int>(ypos));
	}
	void move(std::vector<Stone*>& stones)
	{
		ypos += fall_speed * DBHelper::delta;
		int sum = 0;
		int count = 0;
		for (auto& stone : stones)
		{
			int x = stone->check_isEnter(xpos, ypos);
			if (x!= xpos) 
			{
				sum += x; 
				count++;
			}
		}
		sum += xpos;
		xpos = sum / (count + 1);
		xpos += (-200 + (rand()%500)) * DBHelper::delta;

		fall_speed += DBHelper::delta * 100;
		if (ypos > WINDOW_H)
		{
			randomize();
		}
	}

};

class Waterfall
{
private:
	std::vector<Drop*> drops;
	std::vector<Stone*> stones;
	SDL_Renderer* renderer;
	unsigned MAX_STONE_SIZE;
public:
	Waterfall(SDL_Renderer* renderer, unsigned int MAX_DROPS, unsigned MAX_STONE_SIZE)
	{
		this->renderer = renderer;
		for (unsigned int i = 0 ; i < MAX_DROPS ; i++)
		{
			Drop* drop = new Drop;
			drops.emplace_back(drop);
		}

		for (unsigned int i = 0 ; i < (WINDOW_H * WINDOW_W) * 0.0001; i++)
		{
			Stone* stone = new Stone(MAX_STONE_SIZE);
			stones.emplace_back(stone);
		}
		this->MAX_STONE_SIZE = MAX_STONE_SIZE;
	}
	~Waterfall()
	{
		for (auto& drop : drops)
		{
			delete drop;
		}
	}
	void update_range(int begin, int end)
	{
		for (int i = begin ; i < end ; i++)
		{
			drops[i]->move(stones);
		}
	}
	void update()
	{
		int MAX_THREADS = std::thread::hardware_concurrency();
		std::thread threads[MAX_THREADS];
		for (int i = 0 ; i < MAX_THREADS ; i++)
		{
			threads[i] = std::thread(&Waterfall::update_range, this, drops.size() / MAX_THREADS * i, drops.size() / MAX_THREADS * (i + 1)); 
		}
		for (int i = 0 ; i < MAX_THREADS ; i++)
		{
			threads[i].join();
		}	
		for (auto& drop : drops)
		{
			drop->draw(renderer);
		}
		const Uint8* buf = SDL_GetKeyboardState(NULL);
		if (buf[SDL_SCANCODE_RETURN])
		{
			for (auto& stone : stones)
			{
				stone->randomize(MAX_STONE_SIZE);
			}
		}
		/*for (auto& stone : stones)
		{
			stone->draw(renderer);
		}*/
	}
};

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
	CLI::App app{"desc"};

	unsigned drop_number = 0;
	unsigned stones_size = 0;
	bool isNoFullscreen = false;
	app.add_option("-d,--dropNumber", drop_number, "Number of drops", false)
		->required(true);
	app.add_option("-s,--stonesSize", stones_size, "Stones' size", false)
		->required(true);
	app.add_flag("-f,--isNoFullscreen", isNoFullscreen, "Is window no fullscreen mode?");
		
	CLI11_PARSE(app, argc, argv);

	srand(time(NULL));

	SDL_Init(SDL_INIT_EVERYTHING);

	Uint32 flag = SDL_WINDOW_FULLSCREEN;
	if (isNoFullscreen)
	{
		flag = SDL_WINDOW_SHOWN;
	}
	SDL_Window* window = SDL_CreateWindow("Waterfall", 0, 0, 1920, 1080, flag);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	Waterfall wf(renderer, drop_number, stones_size);

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
		wf.update();
		SDL_RenderPresent(renderer);
		DBHelper::end();
#ifdef DEBUG
		std::cout << "End" << std::endl;
#endif
	}
	handler.join();
	SDL_Quit();
	return 0;
}
