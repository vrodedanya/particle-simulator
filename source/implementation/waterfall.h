#ifndef WATERFALL_H
#define WATERFALL_H

#include "../manager.h"
#include "../particle.h"
#include <vector>
#include <thread>
#include <vector>
#include <cmath>
#include "../dbhelper.h"

class Stone
{
private:
	unsigned& dwidth;
	unsigned& dheight;
public:
	Stone(unsigned& display_width, unsigned& display_height, unsigned max_stone_size = 300) : dwidth(display_width), dheight(display_height)
	{
		randomize(max_stone_size);
	}
	void randomize(unsigned max_stone_size)
	{
		x = rand()%dwidth;
		y = rand()%dheight;
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

class Drop : public Particle 
{
private:
	double xpos;
	double ypos;
	double fall_speed;	
	SDL_Color color;
	bool isBlue;
	std::chrono::system_clock::time_point start;
	unsigned& dwidth;
	unsigned& dheight;
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
		xpos = rand()%dwidth;
		ypos = -(rand()%static_cast<int>(dheight));
		fall_speed = rand()%(static_cast<int>(dheight) / 54);
	}
public:
	Drop(unsigned& display_width, unsigned& display_height) : dwidth(display_width), dheight(display_height)
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
	void update(std::vector<Stone*>& stones)
	{
		ypos += fall_speed * DBHelper::delta;
		fall_speed += DBHelper::delta * (static_cast<int>(dheight) / 10.8);

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
		xpos += (-(dwidth/9.6) + (rand()%static_cast<int>(dwidth / 3.84))) * DBHelper::delta;

		if (ypos > dheight)
		{
			randomize();
		}
	}
};

class Waterfall : public Manager
{
private:
	std::vector<Drop*> drops;
	std::vector<Stone*> stones;
	SDL_Renderer* renderer;
	unsigned MAX_STONE_SIZE;
	unsigned& dwidth;
	unsigned& dheight;
public:
	explicit Waterfall(SDL_Renderer* renderer, unsigned int MAX_DROPS, unsigned MAX_STONE_SIZE, unsigned& width, unsigned& height) : dwidth(width), dheight(height)
	{
		this->renderer = renderer;
		for (unsigned int i = 0 ; i < MAX_DROPS ; i++)
		{
			Drop* drop = new Drop(dwidth, dheight);
			drops.emplace_back(drop);
		}

		for (unsigned int i = 0 ; i < (dwidth * dheight) * 0.0001; i++)
		{
			Stone* stone = new Stone(dwidth, dheight, MAX_STONE_SIZE);
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
			drops[i]->update(stones);
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

#endif
