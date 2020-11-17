#ifndef NET_H
#define NET_H

#include "../manager.h"
#include "../dbhelper.h"
#include <vector>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

class Node
{
public:
	Node() : x(0), y(0), way({1,1}), speed(50 + rand()%150)
	{
	}
	double x;
	double y;
	SDL_Point way;
	double speed;
	int dist(Node& node)
	{
		return dist(node.x, node.y);
	}
	int dist(int xpos, int ypos)
	{
		return (std::sqrt(std::pow(xpos - x, 2) + std::pow(ypos - y, 2)));
	}

	void draw(SDL_Renderer* renderer, int max_radius)
	{
		for (double radius = 0 ; radius < max_radius ; radius++)
		{
			for (double angle = 0 ; angle < 360 ; angle += static_cast<double>(180 / (M_PI * radius)))
			{
				SDL_RenderDrawPoint(renderer, static_cast<int>(x) + std::cos(angle) * radius, static_cast<int>(y) + std::sin(angle) * radius);
			}
		}
	}
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
	unsigned& dwidth;
	unsigned& dheight;
	int radius;
	int way;
public:
	NetManager(SDL_Renderer* rend, unsigned node_count, unsigned& width, unsigned& height) : renderer(rend), dwidth(width), dheight(height), radius(50), way(1)
	{
		for (unsigned i = 0 ; i < node_count ; i++)
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
			node->draw(renderer, radius);
			int min_index = (node != nodes[0]) ? 0 : 1;
			int min = (node != nodes[0]) ? nodes[0]->dist(*node) : nodes[1]->dist(*node);
			for (std::size_t i = 0 ; i < nodes.size() ; i++)
			{
				if (node != nodes[i])
				{
					if (nodes[i]->dist(*node) < min)
					{
						min_index = i;	
						min = nodes[i]->dist(*node);
					}
				}
			}
			SDL_RenderDrawLine(renderer, node->x, node->y, nodes[min_index]->x, nodes[min_index]->y);
		}
	}
	void update() 
	{
		for (auto& node : nodes)
		{
			node->x += node->way.x * node->speed * DBHelper::delta;
			node->y += node->way.y * node->speed * DBHelper::delta;
			if (node->x > 600 || node->x < 0) node->way.x *= -1;
			if (node->y > 600 || node->y < 0) node->way.y *= -1;
		}
	}
	void event_handler(SDL_Event& event)
	{
		if (event.type == SDL_MOUSEWHEEL)
		{
			if (event.wheel.y > 0) radius++;
			else if (radius > 0) radius--;
		}
	}
};

#endif
