#ifndef NET_H
#define NET_H

#include "../manager.h"
#include "../particle.h"
#include <vector>
#include <SDL2/SDL.h>

class net_walker : public Particle
{
private:
	int x;
	int y;
public:
	net_walker()
	{
		x = rand()%1920;
		y = rand()%1080;
	}
	void draw(SDL_Renderer* renderer) override
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		SDL_RenderDrawPoint(renderer, x, y);
	}
	void update() override
	{

	}
};

class NetManager : public Manager
{
private:
	std::vector<SDL_Point*> nodes;
	SDL_Renderer* renderer;
public:
	NetManager(SDL_Renderer* renderer)
	{
		for (int i = 0 ; i < 1000 ; i++)
		{
			SDL_Point* buf = new SDL_Point;
			buf->x = rand()%1920;
			buf->y = rand()%1080;
			nodes.emplace_back(buf);
		}
	}
	~NetManager()
	{
		for (auto& node : nodes)
		{
			delete node;
		}
	}
	void draw() override
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		for (auto& node : nodes)
		{
			SDL_RenderDrawPoint(renderer, node->x, node->y);
		}
	}
};

#endif
