#pragma once
#ifndef __BULLET_H__
#define __BULLET_H__

#include <SDL2/SDL.h>
#include "sprite.h"
#include "../resource_manager.h"

class Bullet : public Sprite
{
public:
	Bullet();
	~Bullet();
	void init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource) ;
	void destroy();
	void draw();
	void update(double ms);
	int getDirection();
	void setDirection(int direction);
	void setPosition(double x, double y);
	void getHitBox(int* x, int* y, int* w, int* h) ;
	void hit();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	ResourceManager* resource;
	SDL_Texture *image;
	int direction;
	double x, y;
};

#endif