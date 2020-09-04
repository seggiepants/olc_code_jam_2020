#pragma once
#ifndef __SPRITE_GROUP_H__
#define __SPRITE_GROUP_H__

#include <SDL2/SDL.h>
#include <vector>
#include "../resource_manager.h"
#include "sprite.h"

class SpriteGroup {
public:
	SpriteGroup();
	~SpriteGroup();
	void init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource);
	void destroy();
	void draw();
	void update(double ms);
	void add(Sprite* item);
	void removeInactive();
	Sprite* operator[](int index);
	int length();
	bool collisionDetect(SpriteGroup* other);
private:
	std::vector<Sprite*> sprites;
	SDL_Window* window;
	SDL_Renderer* renderer;
	ResourceManager* resource;
};

#endif