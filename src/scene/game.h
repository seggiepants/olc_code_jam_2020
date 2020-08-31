#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include "scene.h"

class SceneGame : Scene
{
public:
	SceneGame();
	~SceneGame();
	void init(SDL_Window* window, SDL_Renderer* renderer);
	void destroy();
	void update(double ms);
	void loadMedia();	
	bool running();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Joystick* joystick;
	double cooldown;
	bool up;
	bool down;
	bool left;
	bool right;
	double x;
	double y;
	bool quit;
	std::unordered_map<int, Mix_Chunk*> audio;
	std::unordered_map<int, SDL_Texture*> texture;
	std::unordered_map<int, TTF_Font*> font;
};