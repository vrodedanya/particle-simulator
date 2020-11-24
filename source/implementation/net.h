#ifndef NET_H
#define NET_H

#include "../manager.h"
#include "../dbhelper.h"
#include <vector>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

struct Vector3D
{
	Vector3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_){} 
	double x;
	double y;
	double z;
};

class Node
{
private:
	unsigned& width;
	unsigned& height;
public:
	Node(unsigned& window_width, unsigned& window_height) : width(window_width), height(window_height), x(0), y(0), z(0), way(Vector3D(1,1,1)), speed(height / 50 + rand()%static_cast<int>(height / 40))
	{
		lua_State* lvm = luaL_newstate();
		if(!luaL_dofile(lvm, "../source/implementation/net_settings.lua"))
		{
			lua_getglobal(lvm, "node");
			lua_getfield(lvm, -1, "radius");
			lua_getfield(lvm,-2, "orbit_radius");
			if (lua_isnumber(lvm, -1) && lua_isnumber(lvm, -2))
			{
				radius = lua_tonumber(lvm, -2);
				orbit_radius_ = lua_tonumber(lvm, -1);
				std::cout << radius << " " << orbit_radius_ << std::endl;
			}
			else
			{
				std::cerr << "Check your lua setting file" << std::endl;
				exit(1);
			}
		}
		else
		{
			std::cerr << "Hehe, something is going very bad..." << std::endl;
			std::cerr << lua_tostring(lvm, -1) << std::endl;
			exit(1);
		}
		lua_close(lvm);
	}
	double x;
	double y;
	double z;
	Vector3D way;
	double speed;
	int radius;
	int orbit_radius_;
	int dist(Node& node);
	int dist(int xpos, int ypos, int zpos);
	int dist(int xpos, int ypos);
	void draw(SDL_Renderer* renderer);
};

class NetWalker
{
private:
	unsigned& width;
	unsigned& height;
	bool isInOrbit;
	double xpos;
	double ypos;
	int center_x;
	int center_y;
	double angle;
	int radius;
	Node* node;
    double speed;	
	SDL_Color color;
public:
	NetWalker(unsigned& window_width, unsigned& window_height) : width(window_width), height(window_height), isInOrbit(false), xpos(0), ypos(0), center_x(0), center_y(0), angle(0), radius(0), node(nullptr)
	{
		speed = height / 5 + rand()%static_cast<int>(height/4);
		color.r = rand()%50;
		color.g = rand()%50;
		color.b = 200 + rand()%255;
	}

	void draw(SDL_Renderer* renderer, int radius);
	void update(SDL_Renderer* renderer);
	void change_focus(Node* node){this->node = node;}
	bool isFree(){return isInOrbit;}
	Node* get_focus(){return node;}
};

class NetManager : public Manager
{
private:
	std::vector<Node*> nodes;
	std::vector<NetWalker*> walkers;
	SDL_Renderer* renderer;
	unsigned& dwidth;
	unsigned& dheight;
	int way;
	int node_count;
public:
	NetManager(SDL_Renderer* rend, unsigned node_count, unsigned& width, unsigned& height) : renderer(rend), dwidth(width), dheight(height), way(1)
	{
		SDL_ShowCursor(0);
		this->node_count = node_count;
		for (unsigned i = 0 ; i < node_count ; i++)
		{
			Node* buf = new Node(dwidth, dheight);
			buf->x = rand()%dwidth;
			buf->y = rand()%dheight;
			nodes.emplace_back(buf);
		}
		for (unsigned i = 0 ; i < node_count * 30 ; i++)
		{
			NetWalker* buf = new NetWalker(dwidth, dheight);
			buf->change_focus(nodes[rand()%node_count]);
			walkers.emplace_back(buf);
		}
	}
	~NetManager()
	{
		for (auto& node : nodes)
		{
			delete node;
		}
		for (auto& walker : walkers)
		{
			delete walker;
		}
	}
	void draw();
	void update();
	void event_handler(SDL_Event& event);
};

#endif
