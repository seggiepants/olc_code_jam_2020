#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include "../imgui/imgui.h"
#include "../imgui/imgui_sdl.h"
#include "../imgui/imgui_impl_sdl.h"
#include "scene.h"

class SceneGame : Scene
{
public:
	SceneGame();
	~SceneGame();
	void init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource);
	void destroy();
	void update(double ms);
	void loadMedia();	
	bool running();
	int getNextState();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Joystick* joystick;
	ResourceManager* resource;
	Mix_Chunk* bloop;
	SDL_Texture* tank;
	SDL_Texture* tankBullet;
	TTF_Font* font;
	bool up;
	bool down;
	bool left;
	bool right;
	double x;
	double y;
	bool bulletActive;
	double bulletX;
	double bulletY;
	bool quit;
	bool pause;
};