#include "sand.h"
#include <iostream>

Sand::Sand(double xpos, double ypos)
{
	x = xpos * 5;
	y = ypos * 5;
	fall_speed = 0;
	rect.x = x;
	rect.y = y;
	rect.w = 5;
	rect.h = 5;
	isMoved = false;
}

Sand::~Sand()
{
}

void isIn(std::vector<Sand*> sandbox, Sand* checking, unsigned dwidth, unsigned dheight)
{
	for (auto& sand : sandbox)
	{
		if (sand == checking) continue;
		SDL_Rect new_pos;
		new_pos.w = checking->rect.w;
		new_pos.h = checking->rect.h;
		new_pos.x = checking->rect.x;
		new_pos.y = checking->rect.y + 1;
		if (SDL_HasIntersection(&sand->rect, &new_pos))
		{
			SDL_Rect result;
			SDL_IntersectRect(&sand->rect, &new_pos, &result);
			if (sand->x > checking->x)
			{
				if (sand->x + sand->rect.w > dwidth || sand->isMoved)
				{
					checking->x -= static_cast<double>(result.w);
					checking->isMoved = true;
					sand->isMoved = false;
				}
				else if(checking->x < 0 || checking->isMoved)
				{
					sand->x += static_cast<double>(result.w);
					sand->isMoved = true;
					checking->isMoved = false;
				}
				else
				{
					sand->x += static_cast<double>(result.w / 2);
					checking->x -= static_cast<double>(result.w / 2);
				}
			}
			else
			{
				if (checking->x + checking->rect.w > dwidth || checking->isMoved)
				{
					sand->x -= static_cast<double>(result.w);
					sand->isMoved = true;
					checking->isMoved = false;
				}
				else if(sand->x < 0 || sand->isMoved)
				{
					checking->x += static_cast<double>(result.w);
					checking->isMoved = true;
					sand->isMoved = false;
				}
				else
				{
					checking->x += static_cast<double>(result.w / 2);
					sand->x -= static_cast<double>(result.w / 2);
				}
			}
			if (sand->y > checking->y)
			{
				checking->fall_speed = 0;
				if (sand->y + sand->rect.h > dheight || sand->isMoved)
				{
					checking->y -= static_cast<double>(result.h);
					checking->isMoved = true;
					sand->isMoved = false;
				}
				else if (checking->y < 0 || checking->isMoved)
				{
					sand->y += static_cast<double>(result.h);
					sand->isMoved = true;
					checking->isMoved = false;
				}
				else
				{
					sand->y += static_cast<double>(result.h / 2);
					checking->y -= static_cast<double>(result.h / 2);
				}
			}
			else
			{
				sand->fall_speed = 0;
				if (checking->y + checking->rect.h > dheight || checking->isMoved)
				{
					sand->y -= static_cast<double>(result.h);
					sand->isMoved = true;
					checking->isMoved = false;
				}
				else if (sand->y < 0 || sand->isMoved)
				{
					checking->y += static_cast<double>(result.h);
					checking->isMoved = true;
					sand->isMoved = false;
				}
				else
				{
					checking->y += static_cast<double>(result.h / 2);
					sand->y -= static_cast<double>(result.h / 2);
				}
			}
			return;
		}
	}
}

void Sand::update(std::vector<Sand*> sandbox, unsigned dwidth, unsigned dheight)
{
	if (x >= 0 && y >= 0)
	{
		if (y + rect.h < dheight)
		{
			y += fall_speed * DBHelper::delta;
			fall_speed += 5000 * DBHelper::delta;
		}
		else
		{
			fall_speed = 0;
		}
		rect.x = std::round(x);
		rect.y = std::round(y);
		isIn(sandbox, this, dwidth, dheight);
	}
}

void Sand::draw(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 200, 0, 0);
	SDL_RenderFillRect(renderer, &rect);
}

void SandManager::draw()
{
	for (auto& sand : sandbox)
	{
		sand->draw(renderer);
	}
}

void SandManager::update()
{
	for (auto& sand : sandbox)
	{
		sand->update(sandbox, dwidth, dheight);
	}
}

void SandManager::event_handler(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
			case SDLK_RETURN:
				for (auto& sand : sandbox)
				{
					sand->y = -sand->rect.h;
					sand->x = -sand->rect.w;
					sand->fall_speed = 0;
				}
				break;
		}
	}
	if (event.type == SDL_MOUSEBUTTONDOWN || isPressed)
	{
		isPressed = true;
		for (auto& sand : sandbox)
		{
			if (sand->x < 0 || sand->y < 0 || sand->y > dheight)
			{
				sand->x = event.motion.x;	
				sand->y = event.motion.y;
				sand->fall_speed = 0;
				break;
			}
		}
	}
	if (event.type == SDL_MOUSEBUTTONUP)
	{
		isPressed = false;
	}
		
}
