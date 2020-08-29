#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MOVE_SPEED = 400; // pixels per second.
const double COOLDOWN_MAX = 3000.0;

struct SDL_globals {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	Mix_Chunk* bloop = NULL;
	TTF_Font* font = NULL;
	Uint64 current;
	Uint64 previous;
	Uint64 runtime;
	bool up;
	bool down;
	bool left;
	bool right;
	float x;
	float y;
} globals;

//The image 
SDL_Texture* test;

struct context {
	bool quit;
	SDL_Rect* r;
	double cooldown = (COOLDOWN_MAX / 3.0);
};

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

SDL_Texture* loadTexture(SDL_Renderer* renderer, std::string path)
{
	// Load image at specified path
	SDL_Texture* ret = IMG_LoadTexture(renderer, path.c_str());
	if (ret == NULL)
	{
		std::cout << "Unable to load image \"" << path << "\" SDL Error: " << SDL_GetError() << std::endl;
	}
	
	return ret;
}


bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load default surface
	test = loadTexture(globals.renderer, "assets/image/test.png");
	if (test == NULL)
	{
		std::cout << "Failed to load default image!" << std::endl;
		success = false;
	}

	globals.bloop = Mix_LoadWAV("assets/sound/bloop.ogg");
	if (globals.bloop == NULL) {
		std::cout << "Failed to load sound. Error: \"" << SDL_GetError() << "\"" << std::endl;
		success = false;
	}

	globals.font = TTF_OpenFont("assets/font/NovaSquareBoldOblique.ttf", 14);
	if (globals.font == NULL)
	{
		std::cout << "Failed to load font. Error: \"" << SDL_GetError() << "\"" << std::endl;
		success = false;
	}

	return success;
}

void close()
{
	//Deallocate surface
	SDL_DestroyTexture(test);
	test = NULL;

	Mix_CloseAudio();

	Mix_Quit();

	// SDL_CloseAudio();

	TTF_CloseFont(globals.font);
	globals.font = NULL;

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

void update(void* param) {
	context* arg = (context *)param;
	SDL_Event e;
	float delta, step;
	//Handle events on queue
	globals.previous = globals.current;
	globals.current = SDL_GetPerformanceCounter();
	Uint64 ticks = globals.current - globals.previous;
	globals.runtime = globals.runtime + ticks;
	double ms = (double)((ticks * 1000.0) / SDL_GetPerformanceFrequency());
	double fps = 1000.0 / ms;
	// std::cout << "fps: " << fps << " seconds: " << (globals.runtime / SDL_GetPerformanceFrequency()) << std::endl;

	arg->cooldown = arg->cooldown - ms;
	if (arg->cooldown <= 0) {
		arg->cooldown = COOLDOWN_MAX;
		Mix_PlayChannel(-1, globals.bloop, 0);
	}

	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			arg->quit = true;
		}
		//User presses a key
		else if (e.type == SDL_KEYUP)
		{
			//Select surfaces based on key press
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				globals.up = false;				
				break;

			case SDLK_DOWN:
				globals.down = false;
				break;

			case SDLK_LEFT:
				globals.left = false;
				break;

			case SDLK_RIGHT:
				globals.right = false;
				break;

			case SDLK_ESCAPE:
				arg->quit = true;
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
				globals.up = true;
				break;

			case SDLK_DOWN:
				globals.down = true;
				break;

			case SDLK_LEFT:
				globals.left = true;
				break;

			case SDLK_RIGHT:
				globals.right = true;
				break;

			default:
				std::cout << "[DOWN] Other Key " << e.key.keysym.sym << std::endl;
				break;
			}
		}
	}

	step = (MOVE_SPEED * ms) / 1000.0;
	if (globals.up) {
		globals.y = (float) std::max<float>(0.0, globals.y - step);
	}
	if (globals.down) {
		delta = (float)(SCREEN_HEIGHT - arg->r->h - 1);
		globals.y = (float) std::min<float>(delta, globals.y + step);
	}
	if (globals.left) {
		globals.x = (float) std::max<float>(0.0, globals.x - step);
	}
	if (globals.right) {
		delta = (float)(SCREEN_WIDTH - arg->r->w - 1);
		globals.x = (float) std::min<float>(delta, globals.x + step);
	}

	arg->r->x = (int)globals.x;
	arg->r->y = (int)globals.y;

	//Apply the image
	SDL_RenderClear(globals.renderer);
	SDL_RenderCopy(globals.renderer, test, NULL, arg->r);

	// Write the fps
	char buffer[50];
	snprintf(buffer, 50, "fps: %3.2f", fps);
	SDL_Color fg = { 255, 255, 255 };
	SDL_Surface* fpsSurface = TTF_RenderText_Solid(globals.font, buffer, fg);
	SDL_Texture* fpsTex = SDL_CreateTextureFromSurface(globals.renderer, fpsSurface);
	int width;
	int height;
	SDL_QueryTexture(fpsTex, NULL, NULL, &width, &height);
	SDL_Rect fpsRect;
	fpsRect.x = SCREEN_WIDTH - width - 1;
	fpsRect.y = 0;
	fpsRect.w = width;
	fpsRect.h = height;
	SDL_RenderCopy(globals.renderer, fpsTex, NULL, &fpsRect);
	// std::cout << "Rect x: " << fpsRect.x << " y: " << fpsRect.y << " w: " << fpsRect.w << " h: " << fpsRect.h << std::endl;

	SDL_FreeSurface(fpsSurface);
	SDL_DestroyTexture(fpsTex);

	//Update the surface
	SDL_RenderPresent(globals.renderer);

#ifdef __EMSCRIPTEN__
	if (arg->quit) {
		emscripten_cancel_main_loop();
	}
#endif

}


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
		globals.current = SDL_GetPerformanceCounter();
		globals.runtime = 0;
		globals.up = globals.down = globals.left = globals.right = false;
		
		//Load media
		if (!loadMedia())
		{
			std::cout << "Failed to load media!" << std::endl;
		}
		else
		{
			//Main loop flag
			struct context state;
			state.quit = false;

			//Event handler
			int width;
			int height;
			SDL_QueryTexture(test, NULL, NULL, &width, &height);
			globals.x = 0.0;
			globals.y = 0.0;
			SDL_Rect r = { (int)globals.x, (int)globals.y, width, height };
			state.r = &r;
			

			// While application is running
#ifdef __EMSCRIPTEN__
			emscripten_set_main_loop_arg(update, (void*)&state, -1, 1);
#else
			while (!state.quit) {
				update(&state);
			}
#endif
		}
	}
	return 0;
}