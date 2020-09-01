#include <SDL2/SDL.h>

#pragma once

#ifndef __SCENE_H__
#define __SCENE_H__

class Scene {
public:
	Scene() {};
	virtual ~Scene() {};
	virtual void init(SDL_Window* window, SDL_Renderer* renderer) = 0;
	virtual void destroy() = 0;
	virtual void update(double ms) = 0;
	virtual bool running() = 0;
	virtual int getNextState() = 0;
};

#endif