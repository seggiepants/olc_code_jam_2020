#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include "../3rd_party/imgui/imgui.h"
#include "../3rd_party/imgui/imgui_sdl.h"
#include "../3rd_party/imgui/imgui_impl_sdl.h"
#include "scene.h"
#include "../sprite/sprite_group.h"

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
	void addPoints(int points);
	double getInvaderSpeed();
	void gameOver();
private:
	void initGame();
	void initInvaders();
	void initShields(int y); 
	void initTank();
	void invaderPostUpdate(double ms);
	void drawFPS(double fps, int winW, int winH);
	void drawScore(int winW, int winH);
	void spawnTankBullet();
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Joystick* joystick;
	ResourceManager* resource;
	TTF_Font* font;
	int points;
	double invaderShootDelay;
	double dropDistance;
	double nextDirection;
	bool showFPS;
	bool isGameOver;
	bool quit;
	bool pause;
	bool left;
	bool right;
	Mix_Chunk* bloop;
	Mix_Chunk* laser;
	SpriteGroup tank;
	SpriteGroup tankBullet;
	SpriteGroup invader;
	SpriteGroup invaderBullet;
	SpriteGroup shield;
};