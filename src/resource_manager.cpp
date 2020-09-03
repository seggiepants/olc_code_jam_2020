#include <iostream>
#include "resource_manager.h"

ResourceManager::ResourceManager()
{
	this->Clear();
}

ResourceManager::~ResourceManager()
{
	this->Clear();
}

Mix_Chunk* ResourceManager::GetAudio(std::string path)
{
	if (this->HasAudio(path))
	{
		return this->audio[path];
	}
	else
	{
		Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());

		if (chunk == NULL) {
			std::cout << "Failed to load sound \"" << path << "\". Error: \"" << SDL_GetError() << "\"" << std::endl;
		}
		else
		{
			this->audio[path] = chunk;
		}

		return chunk;
	}
}

TTF_Font* ResourceManager::GetFont(std::string path, int size)
{
	if (this->HasFont(path, size))
	{
		return this->font[path + "_" + std::to_string(size)];
	}
	else
	{
		std::string key = path + "_" + std::to_string(size);
		TTF_Font* font = TTF_OpenFont(path.c_str(), size);
		if (font == NULL)
		{
			std::cout << "Failed to load font \"" << path << "\" Size " << size << ". Error: \"" << SDL_GetError() << "\"" << std::endl;
		}
		else
		{
			this->font[key] = font;
		}
		return font;
	}
}

SDL_Texture* ResourceManager::GetImage(SDL_Renderer* renderer, std::string path)
{
	if (this->HasImage(path))
		return this->image[path];
	else 
	{
		// Load image at specified path
		SDL_Texture* ret = IMG_LoadTexture(renderer, path.c_str());
		if (ret == NULL)
		{
			std::cout << "Unable to load image \"" << path << "\" SDL Error: " << SDL_GetError() << std::endl;
		}
		else {
			this->image[path] = ret;
		}

		return ret;
	}
}

void ResourceManager::Clear()
{
	for (const auto &element : this->audio) {
		Mix_FreeChunk(element.second);
	}
	this->audio.clear();
	
	for (const auto &element : this->font) {
		TTF_CloseFont(element.second);
	}
	this->font.clear();

	for (const auto &element : this->image) {
		SDL_DestroyTexture(element.second);
	}
	this->image.clear();
}

void ResourceManager::FreeAudio(std::string path)
{
	if (this->HasAudio(path)) {
		Mix_FreeChunk(this->audio[path]);
		this->audio.erase(path);
	}
}

void ResourceManager::FreeFont(std::string path, int size)
{	
	if (this->HasFont(path, size)) {
		std::string key = path + "_" + std::to_string(size);
		TTF_CloseFont(this->font[key]);
		this->font.erase(key);
	}
}

void ResourceManager::FreeImage(std::string path)
{
	if (this->HasImage(path)) {
		SDL_DestroyTexture(this->image[path]);
		this->image.erase(path);
	}
}

bool ResourceManager::HasAudio(std::string path)
{
	std::unordered_map<std::string, Mix_Chunk*>::const_iterator search = this->audio.find(path);

	return (search != this->audio.end());

}

bool ResourceManager::HasFont(std::string path, int size)
{
	std::unordered_map<std::string, TTF_Font*>::const_iterator search = this->font.find(path + "_" + std::to_string(size));

	return (search != this->font.end());
}

bool ResourceManager::HasImage(std::string path)
{
	std::unordered_map<std::string, SDL_Texture*>::const_iterator search = this->image.find(path);

	return (search != this->image.end());
}
