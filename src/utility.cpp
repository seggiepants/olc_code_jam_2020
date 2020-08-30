#include <iostream>
#include "utility.h"

SDL_Texture* loadTexture(SDL_Renderer* renderer, std::string path)
{
	// Load image at specified path
	SDL_Texture* ret = IMG_LoadTexture(renderer, path.c_str());
	if (ret == NULL)
	{
		std::cout << "Unable to load image \"" << path << "\" SDL Error: " << SDL_GetError() << std::endl;
	}

	return ret;
}
