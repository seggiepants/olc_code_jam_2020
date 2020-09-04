#pragma once
#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL2/SDL.h>
#include "../resource_manager.h"

class Sprite
{
public:
	Sprite() {};
	virtual ~Sprite() {};
	virtual void init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource) = 0;	
	virtual void destroy() = 0;
	virtual void draw() = 0;
	virtual void update(double ms) = 0;
	virtual void getHitBox(int* x, int* y, int* w, int* h) = 0;
	virtual bool collisionDetect(Sprite* other) = 0;
	virtual void hit() = 0;
	virtual bool getActive() = 0;
	virtual void setActive(bool value) = 0;
	virtual bool getVisible() = 0;
	virtual void setVisible(bool value) = 0;
};

#endif