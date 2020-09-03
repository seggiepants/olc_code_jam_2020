#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

#ifndef __RESOURCE_MANAGER__
#define __RESOURCE_MANAGER__

class ResourceManager {
public:
	ResourceManager();
	~ResourceManager();
	Mix_Chunk* GetAudio(std::string path);
	TTF_Font* GetFont(std::string path, int size);
	SDL_Texture* GetImage(SDL_Renderer* renderer, std::string path);
	void Clear();
	void FreeAudio(std::string);
	void FreeFont(std::string, int size);
	void FreeImage(std::string);
	bool HasAudio(std::string);
	bool HasFont(std::string, int size);
	bool HasImage(std::string);
private:
	std::unordered_map<std::string, Mix_Chunk*> audio;
	std::unordered_map<std::string, SDL_Texture*> image;
	std::unordered_map<std::string, TTF_Font*> font;
};

#endif