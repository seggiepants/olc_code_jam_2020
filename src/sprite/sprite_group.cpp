#include "sprite_group.h"
#include <iostream>

SpriteGroup::SpriteGroup()
{
	this->window = NULL;
	this->renderer = NULL;
	this->resource = NULL;
	this->destroy();
}

SpriteGroup::~SpriteGroup()
{
	this->destroy();
}

void SpriteGroup::init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource)
{
	this->window = window;
	this->renderer = renderer;
	this->resource = resource;
	this->destroy();
}

void SpriteGroup::destroy()
{
	if (this->sprites.size() > 0)
	{
		for (const auto &element : this->sprites) {
			delete element;
		}
		this->sprites.clear();
	}
}

void SpriteGroup::draw()
{
	if (this->sprites.size() > 0)
	{
		for (const auto &element : this->sprites) {
			if (element->getActive() && element->getVisible())
			{
				element->draw();
			}
		}
	}
}

void SpriteGroup::update(double ms)
{
	for (const auto &element : this->sprites) {
		if (element->getActive())
		{
			element->update(ms);
		}
	}
}

void SpriteGroup::add(Sprite* item)
{
	this->sprites.push_back(item);
}

Sprite* SpriteGroup::operator[](int index)
{
	if ((index >= 0) && (index < this->sprites.size())) {
		return this->sprites[index];
	} 
	else
	{
		return NULL;
	}
}

int SpriteGroup::length()
{
	return (int) this->sprites.size();
}

bool SpriteGroup::collisionDetect(SpriteGroup* other)
{
	int i, j;
	Sprite* current;
	bool hit, ret;
	ret = false;
	for (j = 0; j < this->sprites.size(); j++)
	{
		i = 0;
		hit = false;
		current = this->sprites[j];
		while (i < other->length() && hit == false)
		{
			if (current->collisionDetect((*other)[i]))
			{
				ret = true;
				hit = true;
				current->hit();
				(*other)[i]->hit();
				break;
			}
			else
			{
				i++;
			}
		}
	}
	return ret;
}

void SpriteGroup::removeInactive()
{
	std::vector<Sprite*>::iterator i = this->sprites.begin();
	while (i != this->sprites.end()) 
	{ 
		if (!(*i)->getActive())
		{
			i = this->sprites.erase(i);
		}
		else
		{
			++i;
		}
	}
}