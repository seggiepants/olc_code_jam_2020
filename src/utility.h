#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifndef __UTILITY_H__
#define __UTILITY_H__
SDL_Texture* loadTexture(SDL_Renderer* renderer, std::string path);
#endif
