#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifndef __UTILITY_H__
#define __UTILITY_H__

const int JOYSTICK_DEAD_ZONE = 8000;

const int STATE_MENU = 0;
const int STATE_PLAY = 1;
const int STATE_EXIT = 2;

const float FRAME_MIN = (float)(1000.0 / 60.0);

SDL_Texture* loadTexture(SDL_Renderer* renderer, std::string path);
#endif
