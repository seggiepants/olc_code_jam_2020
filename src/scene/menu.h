#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include "../imgui/imgui.h"
#include "../imgui/imgui_sdl.h"
#include "../imgui/imgui_impl_sdl.h"
#include "scene.h"

class SceneMenu : Scene
{
public:
	SceneMenu();
	~SceneMenu();
	void init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource);
	void destroy();
	void update(double ms);
	void loadMedia();
	bool running();
	int getNextState();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	ResourceManager* resource;
	bool quit;
	int nextState;
	SDL_Texture* background;
};