#ifndef NET_H
#define NET_H

#include "../manager.h"
#include "../dbhelper.h"
#include <vector>
#include <SDL2/SDL.h>
#include <iostream>

class Node
{
public:
	double x;
	double y;
};

class net_walker 
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
	void draw(SDL_Renderer* renderer)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		SDL_RenderDrawPoint(renderer, x, y);
	}
	void update() 
	{

	}
};

class NetManager : public Manager
{
private:
	std::vector<Node*> nodes;
	SDL_Renderer* renderer;
	unsigned nodes_count;
	unsigned& dwidth;
	unsigned& dheight;
public:
	NetManager(SDL_Renderer* rend, unsigned& width, unsigned& height) : renderer(rend), dwidth(width), dheight(height) 
	{
		for (int i = 0 ; i < (dwidth * dheight) * 0.000005 ; i++)
		{
			Node* buf = new Node;
			buf->x = rand()%dwidth;
			buf->y = rand()%dheight;
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
	void draw() 
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		for (auto& node : nodes)
		{
			SDL_RenderDrawPoint(renderer, node->x, node->y);
			for (auto& node2 : nodes)
			{
				if (node != node2)
				{
					SDL_RenderDrawLine(renderer, node->x, node->y, node2->x, node2->y);
				}
			}
		}
	}
	void update() 
	{
		for (auto& node : nodes)
		{
			if (node->x > static_cast<int>(dwidth) / 2)
			{
				node->x += 3 * DBHelper::delta;
			}
			else
			{
				node->x -= 3 * DBHelper::delta;
			}
			if (node->y > static_cast<int>(dheight) / 2)
			{
				node->y += 3 * DBHelper::delta;
			}
			else
			{
				node->y -= 3 * DBHelper::delta;
			}
		}
	}
	void event_handler(SDL_Event& event)
	{
	}
};

#endif
