#include <algorithm>
#include <iostream>
#include <string>
#include "tank.h"

const int MOVE_SPEED = 400; // pixels per second.

const std::string PATH_TANK = "assets/image/tank.png";

Tank::Tank()
{
	this->window = NULL;
	this->renderer = NULL;
	this->resource = NULL;
	this->image = NULL;
	this->parent = NULL;
	this->left = this->right = false;
}

Tank::~Tank()
{
	this->destroy();
}

void Tank::init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource)
{
	this->window = window;
	this->renderer = renderer;
	this->resource = resource;
	this->image = this->resource->GetImage(this->renderer, PATH_TANK);	
	this->left = this->right = false;

	int w, h, winW, winH;
	SDL_QueryTexture(this->image, NULL, NULL, &w, &h);
	SDL_GetWindowSize(this->window, &winW, &winH);
	this->x = (winW - w) / 2;
	this->y = winH - (1.5 * h);
}

void Tank::destroy()
{
	// do nothing
}

void Tank::draw()
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

void Tank::update(double ms)
{
	double delta;
	double step;

	if (this->active)
	{
		step = (MOVE_SPEED * ms) / 1000.0;
		int w, h, winW, winH;
		SDL_QueryTexture(this->image, NULL, NULL, &w, &h);
		SDL_GetWindowSize(this->window, &winW, &winH);

		if (this->left) {
			this->x = (double)std::max<double>(0.0, this->x - step);
		}
		if (this->right) {
			delta = (double)(winW - w - 1);
			this->x = (double)std::min<double>(delta, this->x + step);
		}		
	}
}

void Tank::setPosition(double x, double y)
{
	this->x = x;
	this->y = y;
}

void Tank::getHitBox(int* x, int* y, int* w, int* h)
{
	*x = (int) this->x;
	*y = (int) this->y;
	SDL_QueryTexture(this->image, NULL, NULL, w, h);
}

void Tank::getTurrentPosition(int* x, int* y)
{
	int w, h;
	SDL_QueryTexture(this->image, NULL, NULL, &w, &h);
	*x = (int)(this->x + (w / 2));
	*y = (int)(this->y - 1);
}

void Tank::hit()
{
	this->parent->gameOver();
}