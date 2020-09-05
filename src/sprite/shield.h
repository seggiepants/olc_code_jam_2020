#pragma once

#ifndef __SHEILD_H__
#define __SHIELD_H_

#include "sprite.h"

class Shield : Sprite
{
public:
	Shield();
	~Shield();
	void init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource);
	void destroy();
	void update(double ms);
	void draw();
	void getHitBox(int* x, int* y, int* w, int* h);
	void setPosition(double x, double y);
	void hit();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	ResourceManager* resource;
	SDL_Texture* image;
	double x, y;
};

#endif