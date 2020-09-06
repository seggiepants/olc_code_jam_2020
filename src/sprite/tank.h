#pragma once
#ifndef __TANK_H__
#define __TANK_H__

#include "sprite.h"
#include "../scene/game.h"

class Tank : public Sprite
{
public:
	Tank();
	~Tank();
	void init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource);
	void destroy();
	void draw();
	void update(double ms);
	void setInput(bool left, bool right) { this->left = left; this->right = right; }
	void setParent(SceneGame* parent) { this->parent = parent; };
	void setPosition(double x, double y);
	void getHitBox(int* x, int* y, int* w, int* h);
	void getTurrentPosition(int* x, int* y);
	void hit();	
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	ResourceManager* resource;
	SDL_Texture* image;
	SceneGame* parent;
	double x, y;
	bool left, right;
};

#endif