#pragma once
#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL2/SDL.h>
#include "../resource_manager.h"

class Sprite
{
public:
	Sprite() {
		this->active = true;
		this->visible = true;
	};
	virtual ~Sprite() {};
	virtual void init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource) = 0;	
	virtual void destroy() = 0;
	virtual void draw() = 0;
	virtual void update(double ms) = 0;
	virtual void getHitBox(int* x, int* y, int* w, int* h) = 0;
	bool collisionDetect(Sprite* other)
	{
		SDL_Rect src, dest;
		this->getHitBox(&src.x, &src.y, &src.w, &src.h);
		other->getHitBox(&dest.x, &dest.y, &dest.w, &dest.h);
		return SDL_HasIntersection(&src, &dest) == SDL_TRUE;
	};
	virtual void hit() = 0;
	bool getActive() 
	{ 
		return this->active; 
	};
	void setActive(bool value) 
	{
		this->active = value;
	};
	bool getVisible()
	{
		return this->visible;
	};
	void setVisible(bool value)
	{
		this->visible = value;
	};
protected:
	bool active;
	bool visible;
};

#endif