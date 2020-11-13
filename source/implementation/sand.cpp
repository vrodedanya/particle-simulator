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
}

Sand::~Sand()
{
}

bool isIn(std::vector<Sand*> sandbox, Sand* checking)
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
				checking->x -= static_cast<double>(result.w / 2);
				sand->x += static_cast<double>(result.w / 2);
			}
			else
			{
				sand->x -= static_cast<double>(result.w / 2);
				checking->x += static_cast<double>(result.w / 2);
			}
			if (sand->y > checking->y)
			{
				checking->y -= static_cast<double>(result.h / 2);
				sand->y += static_cast<double>(result.h / 2);
			}
			else
			{
				sand->y -= static_cast<double>(result.h / 2);
				checking->y += static_cast<double>(result.h / 2);
			}
			return true;
		}
	}
	return false;
}

void Sand::update(std::vector<Sand*> sandbox, unsigned dheight)
{
	if (x > -1 || y > -1)
	{
		if (!isIn(sandbox, this))
		{
			y += fall_speed * DBHelper::delta;
			if (y + rect.h> dheight)
			{
				fall_speed = 0;
				y--;
			}
			else
			{
				fall_speed += 5000 * DBHelper::delta;
			}
		}
		else
		{

			fall_speed = 0;
		}
	}
	rect.x = x;
	rect.y = y;
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
	const Uint8* code = SDL_GetKeyboardState(NULL);
	if (code[SDL_SCANCODE_SPACE])
	{
		for (auto& sand : sandbox)
		{
			if (sand->x < 0 || sand->y < 0)
			{
				sand->x = event.motion.x;	
				sand->y = event.motion.y;
				sand->fall_speed = 0;
				break;
			}
		}
	}
	if (code[SDL_SCANCODE_RETURN])
	{
		for (auto& sand : sandbox)
		{
			sand->y = -sand->rect.h;
			sand->x = -sand->rect.w;
			sand->fall_speed = 0;
		}
	}
	for (auto& sand : sandbox)
	{
		sand->update(sandbox, dheight);
	}
}

bool SandManager::event_handler()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			return false;
		}
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
		{
			return false;
		}
	}
	return true;
}
