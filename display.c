#include "display.h"

bool initAll(t_display* display)
{
	if (display == NULL) return false;

	return (initSDL() & 
			initSDLImg() & 
			initSDLWindow(display) &
			initSDLFont(display));
}

bool initSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s\n", 
				SDL_GetError());
		return false;
	}
	return true;
}

bool initSDLImg()
{
	int flags = IMG_INIT_JPG|IMG_INIT_PNG;

	if ((flags & IMG_Init(flags)) != flags)
	{
		SDL_Log("IMG_Init: %s\n", IMG_GetError());
		return false;
	}
	
	return true;
}

bool initSDLWindow(t_display* display)
{
	if (display == NULL) return false;

	display->window = SDL_CreateWindow("SPACE INVADERS",
									SDL_WINDOWPOS_CENTERED,
									SDL_WINDOWPOS_CENTERED,
									640, 480, 0);
	if (display->window == NULL)
	{
		SDL_Log("Unable to initialize Window: %s\n", 
				SDL_GetError());
		return false;
	}
	return initSDLRenderer(display);
}

bool initSDLRenderer(t_display* display)
{
	if (display == NULL) return false;

	display->renderer = SDL_CreateRenderer(display->window,
							 -1, SDL_RENDERER_PRESENTVSYNC);
	
	if (display->renderer == NULL)
	{
		SDL_Log("Unable to initialize Renderer: %s\n", 
				SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
	return true;
}

bool initSDLFont(t_display* display)
{
	if (display == NULL) return false;

	if (TTF_Init() == -1) 
	{
		SDL_Log("Unable to initialize TTF_Init: %s\n", 
				TTF_GetError());
		return false;
	}

	display->police = NULL;
    display->police = TTF_OpenFont("space_invaders.ttf", 12);
	
	if (display->police == NULL)
	{
		SDL_Log("Unable to initialize Police: %s\n",
			TTF_GetError());
		return false;
	}

	return true;
}

void clean_display(t_display* display)
{
	if (display == NULL) return;
	
	TTF_CloseFont(display->police);
	SDL_DestroyRenderer(display->renderer);
	SDL_DestroyWindow(display->window);
    TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void clear_display(t_display* display)
{
	if (display == NULL) return;
	
	SDL_RenderClear(display->renderer);
}

void update_display(t_display* display)
{
	if (display == NULL) return;

    SDL_RenderPresent(display->renderer);
}