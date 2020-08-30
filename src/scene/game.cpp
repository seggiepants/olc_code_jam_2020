#include <iostream>
#include <algorithm>
#include "game.h"
#include "../utility.h"

const int MOVE_SPEED = 400; // pixels per second.
const double COOLDOWN_MAX = 3000.0;

const int FONT_DEFAULT = 0;

const int SOUND_BLOOP = 0;

const int TEXTURE_TEST = 0;

SceneGame::SceneGame() {
	this->quit = false;
	this->up = this->down = this->left = this->right = false;
	this->x = this->y = 0.0;
	this->cooldown = 0;
}

SceneGame::~SceneGame() {
	this->destroy();
}

void SceneGame::loadMedia() {
	Mix_Chunk* chunk = Mix_LoadWAV("assets/sound/bloop.ogg");

	if (chunk == NULL) {
		std::cout << "Failed to load sound. Error: \"" << SDL_GetError() << "\"" << std::endl;
		this->quit = true;
	}
	this->audio[SOUND_BLOOP] = chunk;

	SDL_Texture* texture = loadTexture(this->renderer, "assets/image/test.png");
	if (texture == NULL)
	{
		std::cout << "Failed to load default image!" << std::endl;
		this->quit = true;
	}
	this->texture[TEXTURE_TEST] = texture;

	TTF_Font* font = TTF_OpenFont("assets/font/NovaSquareBoldOblique.ttf", 14);
	if (font == NULL)
	{
		std::cout << "Failed to load font. Error: \"" << SDL_GetError() << "\"" << std::endl;
		this->quit = true;
	}
	this->font[FONT_DEFAULT] = font;
}

void SceneGame::init(SDL_Window* window, SDL_Renderer* renderer) {
	this->window = window;
	this->renderer = renderer;
	this->quit = false;
	this->up = this->down = this->left = this->right = false;
	this->x = this->y = 0.0;
	this->cooldown = COOLDOWN_MAX / 3.0;
	this->audio.clear();
	this->texture.clear();
	this->font.clear();
	this->loadMedia();
}

void SceneGame::destroy() {
	for (const auto &element : this->audio) {
		Mix_FreeChunk(element.second);
	}

	for (const auto &element : this->texture) {
		SDL_DestroyTexture(element.second);
	}
}

void SceneGame::update(double ms) {
	double fps = 1000.0 / ms;
	// std::cout << "fps: " << fps << " seconds: " << (globals.runtime / SDL_GetPerformanceFrequency()) << std::endl;
	SDL_Event e;
	double delta;
	double step;

	this->cooldown -= ms;
	if (this->cooldown <= 0) {
		this->cooldown = COOLDOWN_MAX;
		Mix_PlayChannel(-1, this->audio[SOUND_BLOOP], 0);
	}

	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			this->quit = true;
		}
		//User presses a key
		else if (e.type == SDL_KEYUP)
		{
			//Select surfaces based on key press
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				this->up = false;
				break;

			case SDLK_DOWN:
				this->down = false;
				break;

			case SDLK_LEFT:
				this->left = false;
				break;

			case SDLK_RIGHT:
				this->right = false;
				break;

			case SDLK_ESCAPE:
				this->quit = true;
				break;

			default:
				std::cout << "[UP] Other Key " << e.key.keysym.sym << std::endl;
				break;
			}
		}
		else if (e.type == SDL_KEYDOWN)
		{
			//Select surfaces based on key press
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				this->up = true;
				break;

			case SDLK_DOWN:
				this->down = true;
				break;

			case SDLK_LEFT:
				this->left = true;
				break;

			case SDLK_RIGHT:
				this->right = true;
				break;

			default:
				std::cout << "[DOWN] Other Key " << e.key.keysym.sym << std::endl;
				break;
			}
		}
	}

	step = (MOVE_SPEED * ms) / 1000.0;
	SDL_Texture* test = this->texture[TEXTURE_TEST];
	int w, h, winW, winH;
	SDL_QueryTexture(test, NULL, NULL, &w, &h);
	SDL_GetWindowSize(this->window, &winW, &winH);
	if (this->up) {
		this->y = (double)std::max<double>(0.0, this->y - step);
	}
	if (this->down) {
		delta = (double)(winH - h - 1);
		this->y = (double)std::min<double>(delta, this->y + step);
	}
	if (this->left) {
		this->x = (double)std::max<double>(0.0, this->x - step);
	}
	if (this->right) {
		delta = (double)(winW - w - 1);
		this->x = (double)std::min<double>(delta, this->x + step);
	}
	SDL_Rect r;
	r.x = (int) this->x;
	r.y = (int) this->y;
	r.w = w;
	r.h = h;

	//Apply the image
	SDL_RenderClear(this->renderer);
	SDL_RenderCopy(this->renderer, test, NULL, &r);

	// Write the fps
	char buffer[50];
	snprintf(buffer, 50, "fps: %3.2f", fps);
	SDL_Color fg = { 255, 255, 255 };
	SDL_Surface* fpsSurface = TTF_RenderText_Solid(this->font[FONT_DEFAULT], buffer, fg);
	SDL_Texture* fpsTex = SDL_CreateTextureFromSurface(this->renderer, fpsSurface);
	int width;
	int height;
	SDL_QueryTexture(fpsTex, NULL, NULL, &width, &height);
	SDL_Rect fpsRect;
	fpsRect.x = winW - width - 1;
	fpsRect.y = 0;
	fpsRect.w = width;
	fpsRect.h = height;
	SDL_RenderCopy(this->renderer, fpsTex, NULL, &fpsRect);
	// std::cout << "Rect x: " << fpsRect.x << " y: " << fpsRect.y << " w: " << fpsRect.w << " h: " << fpsRect.h << std::endl;

	SDL_FreeSurface(fpsSurface);
	SDL_DestroyTexture(fpsTex);

	//Update the surface
	SDL_RenderPresent(this->renderer);
}

bool SceneGame::running() {
	return !this->quit;
}