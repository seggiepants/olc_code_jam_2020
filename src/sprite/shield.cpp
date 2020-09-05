#include <string>
#include "shield.h"

const std::string PATH_SHIELD = "assets/image/shield.png";

Shield::Shield() : Sprite()
{
	this->window = NULL;
	this->renderer = NULL;
	this->resource = NULL;
	this->image = NULL;
	this->x = this->y = 0.0;
}

Shield::~Shield()
{
	this->destroy();
}

void Shield::init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource)
{
	this->active = true;
	this->visible = true;
	this->window = window;
	this->renderer = renderer;
	this->resource = resource;
	this->image = this->resource->GetImage(this->renderer, PATH_SHIELD);
}

void Shield::destroy()
{
	// Do nothing
}

void Shield::draw()
{
	SDL_Rect r;
	if (this->active && this->getVisible())
	{
		SDL_QueryTexture(this->image, NULL, NULL, &r.w, &r.h);
		r.x = (int)this->x;
		r.y = (int)this->y;
		SDL_RenderCopy(this->renderer, this->image, NULL, &r);
	}
}

void Shield::update(double ms)
{
	// Do nothing
}

void Shield::setPosition(double x, double y)
{
	this->x = x;
	this->y = y;
}

void Shield::getHitBox(int* x, int* y, int* w, int* h)
{
	int width, height;
	SDL_QueryTexture(this->image, NULL, NULL, &width, &height);
	*x = (int)this->x;
	*y = (int)this->y;
	*w = width;
	*h = height;
}

void Shield::hit()
{
	this->setActive(false);
}
