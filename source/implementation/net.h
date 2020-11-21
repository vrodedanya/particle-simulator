#ifndef NET_H
#define NET_H

#include "../manager.h"
#include "../dbhelper.h"
#include <vector>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

struct Vector3D
{
	Vector3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_){} 
	double x;
	double y;
	double z;
};

class Node
{
public:
	Node() : x(0), y(0), z(0), way(Vector3D(1,1,1)), speed(50 + rand()%150)
	{
	}
	double x;
	double y;
	double z;
	Vector3D way;
	double speed;
	int dist(Node& node)
	{
		return dist(node.x, node.y, node.z);
	}
	int dist(int xpos, int ypos, int zpos)
	{
		return (std::sqrt(std::pow(xpos - x, 2) + std::pow(ypos - y, 2) + std::pow(zpos - z, 2)));
	}

	void draw(SDL_Renderer* renderer, int max_radius)
	{
		double real_radius = -z * static_cast<double>(max_radius - 20) / 600 + max_radius;
		for (int y_ = y - real_radius ; y_ < y + real_radius ; y_++)
		{
			for (int x_ = x - real_radius ; x_ < x + real_radius ; x_++)
			{
				if (std::pow(x_ - x, 2) + std::pow(y_ - y, 2) <= std::pow(real_radius, 2))
				{
					SDL_RenderDrawPoint(renderer, x_, y_); 
				}
			}
		}
	}
	void drawLineTo(SDL_Renderer* renderer, const Node& node, int width)	
	{
		Node n1;
		Node n2;
		if (z > node.z)
		{
			n1 = *this;
			n2 = node;
		}
		else
		{
			n1 = node;
			n2 = *this;
		}
		double prefix = (1 - static_cast<double>(std::abs(z - node.z)) / 600);

		for (int i = -(width / 2) ; i < width / 2 ; i++)
		{
			//SDL_RenderDrawLine(renderer, x + i, y, node.x + (width * (1 - (std::abs(z - node.z) / 600))), node.y);
			//if (i != 0)SDL_RenderDrawLine(renderer, x, y + i, node.x, node.y + (width * (1 - std::abs(z - node.z) / 600)));
			SDL_RenderDrawLine(renderer, n1.x + i, n1.y, n2.x + i * prefix, n2.y);
			if (i != 0) SDL_RenderDrawLine(renderer, n1.x, n1.y + i, n2.x, n2.y + i * prefix);
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
			node->drawLineTo(renderer, *nodes[min_index], 10);
		}
	}
	void update() 
	{
		for (auto& node : nodes)
		{
			node->x += node->way.x * node->speed * DBHelper::delta;
			node->y += node->way.y * node->speed * DBHelper::delta;
			node->z += node->way.z * node->speed * DBHelper::delta;

			if (node->x + radius + node->z / 20 > dwidth || node->x - radius - node->z / 20 < 0) node->way.x *= -1;
			if (node->y + radius + node->z / 20 > dheight || node->y - radius - node->z / 20 < 0) node->way.y *= -1;
			if (node->z > 600 || node->z < 0) node->way.z *= -1;
		}
	}
	void event_handler(SDL_Event& event)
	{
		if (event.type == SDL_MOUSEWHEEL)
		{
			if (event.wheel.y > 0) radius++;
			else if (radius > 30) radius--;
		}
	}
};

#endif
