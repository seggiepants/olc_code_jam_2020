#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"
#include "scene/game.h"
#include "scene/menu.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

struct SDL_globals {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	Scene* currentScene = NULL;
#ifdef __EMSCRIPTEN__
	Uint64 current = 0;
	Uint64 previous = 0;
	double ms = 0.0;
#endif
} globals;


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		success = false;
	}
	else
	{
		//Create window
		globals.window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (globals.window == NULL)
		{
			std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
			success = false;
		}
		else
		{
			globals.renderer = SDL_CreateRenderer(globals.window, -1, SDL_RENDERER_ACCELERATED);
			SDL_SetRenderDrawColor(globals.renderer, 32, 32, 255, 255);

			/* Clear the entire screen to our selected color. */
			SDL_RenderClear(globals.renderer);
		}

		/*
		SDL_AudioSpec want, have;

		SDL_memset(&want, 0, sizeof(want)); // or SDL_zero(want) 
		want.freq = 48000;
		want.format = AUDIO_F32;
		want.channels = 2;
		want.samples = 4096;
		want.callback = NULL; // ummm maybe later, see SDL_AudioSpec 

		if (SDL_OpenAudio(&want, &have) < 0) {
			SDL_Log("Failed to open audio: %s", SDL_GetError());
		}
		else {
			if (have.format != want.format) {
				SDL_Log("We didn't get Float32 audio format.");
			}
			SDL_PauseAudio(0); // start audio playing.
		}
		*/

		if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) < 0) {
			std::cout << "Sound mixer could not be initialized: \"" << SDL_GetError() << "\"" << std::endl;
			success = false;
		}

		if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512) < 0) {
			std::cout << "Sound mixer could not be opened: \"" << SDL_GetError() << "\"" << std::endl;
			success = false;
		}


		if (TTF_Init() < 0) {
			std::cout << "Font system could not be initialized : \"" << TTF_GetError() << "\"" << std::endl;
			success = false;
		}
	}

	return success;
}

void close()
{
	globals.currentScene->destroy();
	
	Mix_CloseAudio();
	Mix_Quit();

	TTF_Quit();

#ifdef __EMSCRIPTEN__
	std::cout << "Shutting down" << std::endl;
#else
	//Destroy window	
	SDL_DestroyRenderer(globals.renderer);
	globals.renderer = NULL;

	SDL_DestroyWindow(globals.window);
	globals.window = NULL;

	//Quit SDL subsystems
	SDL_Quit();
#endif
	
}

#ifdef __EMSCRIPTEN__
void emscripten_update(void* param) {
	globals.previous = globals.current;
	globals.current = SDL_GetPerformanceCounter();
	Uint64 ticks = globals.current - globals.previous;
	globals.ms = (double)((ticks * 1000.0) / SDL_GetPerformanceFrequency());

	globals.currentScene->update(globals.ms);
	if (!globals.currentScene->running()) {
		emscripten_cancel_main_loop();
	}
}
#endif

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		std::cout << "Failed to initialize!" << std::endl;
	}
	else
	{
		//Free resources and close SDL on exit
		atexit(close);		
		
		globals.currentScene = (Scene*) (new SceneGame());
		globals.currentScene->init(globals.window, globals.renderer);
		//Load media
		if (!globals.currentScene->running())
		{
			std::cout << "Failed to load!" << std::endl;
		}
		else
		{
			// While application is running
#ifndef __EMSCRIPTEN__
			Uint64 current, previous;
			double ms;
			ms = 0.0;
			current = SDL_GetPerformanceCounter();
			previous = 0;
#endif
				
#ifdef __EMSCRIPTEN__
			emscripten_set_main_loop_arg(emscripten_update, (void*)NULL, -1, 1);
#else
			while (globals.currentScene->running()) {
				previous = current;
				current = SDL_GetPerformanceCounter();
				Uint64 ticks = current - previous;
				double ms = (double)((ticks * 1000.0) / SDL_GetPerformanceFrequency());
				globals.currentScene->update(ms);
			}
#endif
		}
	}
	return 0;
}