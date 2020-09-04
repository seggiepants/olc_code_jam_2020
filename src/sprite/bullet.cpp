#include <string>
#include "bullet.h"

const int DIRECTION_UP = -1;
const int DIRECTION_DOWN = 1;
const double BULLET_SPEED = 300.0;

const std::string PATH_BULLET_UP = "assets/image/tank_bullet.png";
const std::string PATH_BULLET_DOWN = "assets/image/laser.png";

Bullet::Bullet() 
{
	this->active = true;
	this->visible = true;
	this->window = NULL;
	this->renderer = NULL;
	this->resource = NULL;
	this->image = NULL;
	this->direction = DIRECTION_DOWN;

}

Bullet::~Bullet() 
{
	this->destroy();
}

void Bullet::init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource)
{
	this->active = true;
	this->visible = true;
	this->window = window;
	this->renderer = renderer;
	this->resource = resource;
	this->setDirection(DIRECTION_DOWN);
}

void Bullet::destroy()
{
	// Do nothing
}

void Bullet::draw()
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

void Bullet::update(double ms)
{
	SDL_Rect r;
	int winW, winH;
	if (this->active)
	{
		SDL_QueryTexture(this->image, NULL, NULL, &r.w, &r.h);
		SDL_GetWindowSize(this->window, &winW, &winH);
		this->y += (BULLET_SPEED * ms * this->direction) / 1000.0;
		if ((this->y + r.h < 0) || (this->y >= winH))
		{
			this->setActive(false);
		}
	}
}

int Bullet::getDirection()
{
	return this->direction;
}

void Bullet::setDirection(int direction)
{
	if (direction < 0) 
	{
		this->direction = DIRECTION_UP;
		this->image = this->resource->GetImage(this->renderer, PATH_BULLET_UP);
	}
	else if (direction == 0)
	{
		this->direction = 0;
		this->image = this->resource->GetImage(this->renderer, PATH_BULLET_UP);
	}
	else
	{
		this->direction = DIRECTION_DOWN;
		this->image = this->resource->GetImage(this->renderer, PATH_BULLET_DOWN);
	}
}

void Bullet::setPosition(double x, double y)
{
	this->x = x;
	this->y = y;
}

void Bullet::getHitBox(int* x, int* y, int* w, int* h)
{
	int width, height;
	SDL_QueryTexture(this->image, NULL, NULL, &width, &height);
	*x = (int)this->x;
	*y = (int)this->y;
	*w = width;
	*h = height;
}

bool Bullet::collisionDetect(Sprite* other)
{
	SDL_Rect src, dest;
	this->getHitBox(&src.x, &src.y, &src.w, &src.h);
	other->getHitBox(&dest.x, &dest.y, &dest.w, &dest.h);
	return SDL_HasIntersection(&src, &dest) == SDL_TRUE;
}

void Bullet::hit()
{
	this->setActive(false);
}

bool Bullet::getActive()
{
	return this->active;
}

void Bullet::setActive(bool value)
{
	this->active = value;
}

bool Bullet::getVisible()
{
	return this->visible;
}

void Bullet::setVisible(bool value)
{
	this->visible = value;
}

