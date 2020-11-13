#include "sand.h"
#include <iostream>

Sand::Sand(double xpos, double ypos)
{
	x = xpos;
	y = ypos;
	fall_speed = 0;
}

Sand::~Sand()
{
}

void Sand::update()
{
	if (x > -1 || y > -1)
	{
		y += fall_speed * DBHelper::delta;
		fall_speed += 5000 * DBHelper::delta;
	}
}

void Sand::draw(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	SDL_RenderDrawPoint(renderer, x, y);
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
		sand->update();
	}
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
			if (sand->y > static_cast<int>(dheight))
			{
				sand->y--;
				sand->fall_speed = 0;
			}
		}
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
