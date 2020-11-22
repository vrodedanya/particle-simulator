#include "net.h"

int Node::dist(Node& node)
{
	return dist(node.x, node.y, node.z);
}
int Node::dist(int xpos, int ypos, int zpos)
{
	return (std::sqrt(std::pow(xpos - x, 2) + std::pow(ypos - y, 2) + std::pow(zpos - z, 2)));
}
int Node::dist(int xpos, int ypos)
{
	return (std::sqrt(std::pow(xpos - x, 2) + std::pow(ypos - y, 2)));
}

void Node::draw(SDL_Renderer* renderer)
{
//	radius = -z * static_cast<double>(max_radius - 20) / 600 + max_radius;
	for (int y_ = y - radius ; y_ < y + radius ; y_++)
	{
		for (int x_ = x - radius ; x_ < x + radius; x_++)
		{
			if (std::pow(x_ - x, 2) + std::pow(y_ - y, 2) <= std::pow(radius, 2))
			{
				SDL_RenderDrawPoint(renderer, x_, y_); 
			}
		}
	}
}


void NetManager::draw() 
{
	SDL_SetRenderDrawColor(renderer, 200, 200, 200, 0);
	for (auto& node : nodes)
	{
		node->draw(renderer);
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
	}
}
void NetManager::update() 
{
	for (auto& node : nodes)
	{
		node->x += node->way.x * node->speed * DBHelper::delta;
		node->y += node->way.y * node->speed * DBHelper::delta;

		if (node->x + node->radius + node->z / 20 > dwidth || node->x - node->radius - node->z / 20 < 0) node->way.x *= -1;
		if (node->y + node->radius + node->z / 20 > dheight || node->y - node->radius - node->z / 20 < 0) node->way.y *= -1;
	}
	for (auto& walker : walkers)
	{
		walker->update(renderer);
	}
}
void NetManager::event_handler(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_RETURN)
		{
			for (auto& walker : walkers)
			{
				walker->change_focus(nodes[rand()%node_count]);
			}
		}
	}
}

void NetWalker::draw(SDL_Renderer* renderer, int radius)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0);
	for (int y_ = ypos - radius ; y_ < ypos + radius ; y_++)
	{
		for (int x_ = xpos - radius ; x_ < xpos + radius; x_++)
		{
			if (std::pow(x_ - xpos, 2) + std::pow(y_ - ypos, 2) <= std::pow(radius, 2))
			{
				SDL_RenderDrawPoint(renderer, x_, y_); 
			}
		}
	}
}


void NetWalker::update(SDL_Renderer* renderer) 
{
	if (node != nullptr)
	{
		if (node->dist(xpos, ypos) > node->radius + 55)
		{
			xpos += (node->x - xpos) / node->dist(xpos, ypos) * speed * DBHelper::delta;
			ypos += (node->y - ypos) / node->dist(xpos, ypos) * speed * DBHelper::delta;
			radius = 0;
			angle = 180 - std::asin((node->y - ypos) / node->dist(xpos, ypos));
			isInOrbit = false;
		}
		else
		{
			if (!isInOrbit) radius = node->radius + 10 + rand()%50;
			angle += (speed / 100) * DBHelper::delta;
			isInOrbit = true;
			xpos = node->x + std::cos(angle) * radius;
			ypos = node->y + std::sin(angle) * radius;
		}
	}
	draw(renderer, 3);
}
