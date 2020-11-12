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

	int x;
	int y;
	int radius;

public:
	Stone(unsigned& display_width, unsigned& display_height, unsigned max_stone_size = 300) : dwidth(display_width), dheight(display_height)
	{
		randomize(max_stone_size);
	}
	void randomize(unsigned max_stone_size);
	int check_isEnter(int xpos, int ypos);
	void draw(SDL_Renderer* renderer);
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
	void randomize();
	std::vector<Stone*>& stones;
public:
	Drop(unsigned& display_width, unsigned& display_height, std::vector<Stone*>& st) : dwidth(display_width), dheight(display_height), stones(st)
	{
		start = std::chrono::system_clock::now();
		isBlue = true;
		randomize();
	}
	~Drop()
	{
	}

	void draw(SDL_Renderer* renderer);
	void update();
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
		for (unsigned int i = 0 ; i < (dwidth * dheight) * 0.0001; i++)
		{
			Stone* stone = new Stone(dwidth, dheight, MAX_STONE_SIZE);
			stones.emplace_back(stone);
		}

		for (unsigned int i = 0 ; i < MAX_DROPS ; i++)
		{
			Drop* drop = new Drop(dwidth, dheight, stones);
			drops.emplace_back(drop);
		}
		this->MAX_STONE_SIZE = MAX_STONE_SIZE;
	}
	~Waterfall()
	{
		for (auto& drop : drops)
		{
			delete drop;
		}
		for (auto& stone : stones)
		{
			delete stone;
		}
	}
	void update_range(int begin, int end);
	void update();
	void draw();
};

#endif
