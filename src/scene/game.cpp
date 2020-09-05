#include <iostream>
#include <algorithm>
#include "game.h"
#include "../utility.h"
#include "../sprite/bullet.h"
#include "../sprite/shield.h"

const int MOVE_SPEED = 400; // pixels per second.
const int FONT_DEFAULT = 0;
const int SOUND_BLOOP = 0;
const int TEXTURE_TANK = 0;

const std::string PATH_BLOOP = "assets/sound/bloop.ogg";
const std::string PATH_TANK = "assets/image/tank.png";
const std::string PATH_FONT = "assets/font/NovaSquareBoldOblique.ttf";
const int FONT_SIZE = 14;

SceneGame::SceneGame() {
	this->quit = false;
	this->up = this->down = this->left = this->right = false;
	this->x = this->y = 0.0;
	this->joystick = NULL;
	this->pause = false;
}

SceneGame::~SceneGame() {
	this->destroy();
}

int SceneGame::getNextState() {
	return STATE_MENU;
}

void SceneGame::loadMedia() {
	this->bloop = resource->GetAudio(PATH_BLOOP);
	this->font = resource->GetFont(PATH_FONT, FONT_SIZE);
	this->tank = resource->GetImage(this->renderer, PATH_TANK);

	if (this->bloop == NULL || this->font == NULL || this->tank == NULL)
		this->quit = true;
}

void SceneGame::init(SDL_Window* window, SDL_Renderer* renderer, ResourceManager* resource) {
	this->window = window;
	this->renderer = renderer;
	this->resource = resource;
	this->joystick = NULL;
	if (SDL_NumJoysticks() >= 1)
	{
		//Load joystick
		this->joystick = SDL_JoystickOpen(0);
		if (this->joystick == NULL)
		{
			std::cout << "Warning: Unable to open game controller! SDL Error: \"" << SDL_GetError()  << "\"" << std::endl;
		}
	}
	this->quit = false;
	this->up = this->down = this->left = this->right = false;
	this->x = this->y = 0.0;
	this->pause = false;
	this->loadMedia();
	this->tankBullet.init(this->window, this->renderer, this->resource);
	this->shield.init(this->window, this->renderer, this->resource);
	int winW, winH;
	SDL_GetWindowSize(this->window, &winW, &winH);
	int w, h;
	SDL_QueryTexture(this->tank, NULL, NULL, &w, &h);
	this->initShields(winH - (2.5 * h));
	this->x = (winW - w) / 2;
	this->y = winH - (1.5 * h);
}

void SceneGame::destroy() {
	this->tankBullet.destroy();
	this->shield.destroy();
	//Close game controller
	if (this->joystick != NULL) {
		SDL_JoystickClose(this->joystick);
		this->joystick = NULL;
	}
}

void SceneGame::update(double ms) {
	double fps = 1000.0 / ms;
	// std::cout << "fps: " << fps << " seconds: " << (globals.runtime / SDL_GetPerformanceFrequency()) << std::endl;
	SDL_Event e;
	double delta;
	double step;
	SDL_Rect r;
	int w, h, winW, winH;
	bool showGUI = this->pause;
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	int wheel = 0;

	while (SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			this->quit = true;
		}
		else if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				io.DisplaySize.x = static_cast<float>(e.window.data1);
				io.DisplaySize.y = static_cast<float>(e.window.data2);
			}
		}
		else if (e.type == SDL_MOUSEWHEEL)
		{
			wheel = e.wheel.y;
		}
		//User presses a key
		if (e.type == SDL_KEYUP)
		{
			if (io.WantCaptureKeyboard) {
				io.KeysDown[e.key.keysym.scancode] = false;
				// std::cout << "Key Up: " << e.key.keysym.scancode << std::endl;
			}
			
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

			case SDLK_SPACE:
				if (!showGUI)
				{
					this->spawnTankBullet();
				}
				break;

			case SDLK_p:
				this->pause = !this->pause;
				break;

			default:
				//std::cout << "[UP] Other Key " << e.key.keysym.sym << std::endl;
				break;
			}
		}
		else if (e.type == SDL_KEYDOWN)
		{
			if (io.WantCaptureKeyboard) {
				io.KeysDown[e.key.keysym.scancode] = true;
			}
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
				// std::cout << "[DOWN] Other Key " << e.key.keysym.sym << std::endl;
				break;
			}
		}
		else if (e.type == SDL_JOYAXISMOTION)
		{
			//Motion on controller 0
			if (e.jaxis.which == 0)
			{
				//X axis motion
				if (e.jaxis.axis == 0)
				{
					//Left of dead zone
					if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						this->left = true;
					}
					//Right of dead zone
					else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						this->right = true;
					}
					else
					{
						this->left = this->right = false;
					}
				}
				//Y axis motion
				else if (e.jaxis.axis == 1)
				{
					//Below of dead zone
					if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						this->up = true;
					}
					//Above of dead zone
					else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						this->down = true;
					}
					else
					{
						this->up = this->down = false;
					}
				}
			}
		}
		else if (e.type == SDL_JOYBUTTONDOWN)
		{
			if (e.jbutton.which == 0)
			{
				//std::cout << "Joystick button down. Button #" << (int)e.jbutton.button << std::endl;
			}
		}
		else if (e.type == SDL_JOYBUTTONUP) {
			if (e.jbutton.which == 0)
			{
				if (e.jbutton.button == 0) {
					this->spawnTankBullet();
				}
				else if (e.jbutton.button == 1) {
					this->quit = true;
				}
				else {
					//std::cout << "Joystick button up. Button #" << (int)e.jbutton.button << std::endl;
				}
			}
		}
	}

	int mouseX, mouseY;
	const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
	// Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)

	io.DeltaTime = std::max<float>(FRAME_MIN, (float) ms);
	io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io.MouseWheel = static_cast<float>(wheel);

	SDL_QueryTexture(this->tank, NULL, NULL, &w, &h); 
	SDL_GetWindowSize(this->window, &winW, &winH);
	

	if (!showGUI) {
		step = (MOVE_SPEED * ms) / 1000.0;		
				
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
		this->tankBullet.removeInactive();	
		this->tankBullet.update(ms);
		this->shield.removeInactive();
		this->shield.update(ms);
		this->shield.collisionDetect(&this->tankBullet);
	}	
	
	if (showGUI) {
		// Draw a pause message box
		ImGui::NewFrame();
		ImGui::SetNextWindowPos(ImVec2((float)(winW / 3.0), (float)(winH / 3.0)), ImGuiCond_::ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2((float)(winW / 3.0), (float)(winH / 8.0)), ImGuiCond_::ImGuiCond_Once);
		ImGui::Begin("Pause");
		ImGui::Text("Paused...");
		if (ImGui::Button("OK")) {
			this->pause = false;
		}
		ImGui::End();
		ImGui::EndFrame();		
		ImGui::Render();
	}

	SDL_RenderClear(this->renderer);

	SDL_Rect fillR;
	fillR.x = 0;
	fillR.y = 0;
	fillR.w = winW;
	fillR.h = winH;


	SDL_SetRenderDrawColor(this->renderer, 64, 64, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(this->renderer, &fillR);
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	//if (showGUI) ImGui::Render();

	//Apply the image
	// SDL_RenderClear(this->renderer);

	r.x = (int)this->x;
	r.y = (int)this->y;
	r.w = w;
	r.h = h;

	SDL_RenderCopy(this->renderer, this->tank, NULL, &r);

	this->shield.draw();
	this->tankBullet.draw();
	
	this->drawFPS(fps, winW, winH);

	if (showGUI) {
		// Draw a pause message box

		// Rendering		
		ImGuiSDL::Render(ImGui::GetDrawData());

	}

	//Update the surface
	SDL_RenderPresent(this->renderer);	
}

bool SceneGame::running() {
	return !this->quit;
}

void SceneGame::spawnTankBullet()
{
	Bullet* bullet = new Bullet();
	bullet->init(this->window, this->renderer, this->resource);
	bullet->setDirection(-1);
	int bulletX, bulletY, bulletW, bulletH, w, h;
	bullet->setPosition(0.0, 0.0);
	bullet->getHitBox(&bulletX, &bulletY, &bulletW, &bulletH);
	SDL_QueryTexture(this->tank, NULL, NULL, &w, &h);
	bullet->setPosition(this->x + ((w - bulletW) / 2), this->y - bulletH - 1);
	this->tankBullet.add((Sprite*)bullet);
	Mix_PlayChannel(-1, this->bloop, 0);
}

void SceneGame::initShields(int y)
{
	const int NUM_SHIELDS = 4;
	bool skip;
	int x, temp, iStep = 0, jStep = 0;
	int corner_w, corner_h;
	Shield* chunk;
	double x1, y1;

	int winW, winH;
	SDL_GetWindowSize(this->window, &winW, &winH);

	int shield_step = winW / (4 * NUM_SHIELDS);
	int shield_w = shield_step * 2;
	int shield_h = shield_step;
	corner_w = (shield_w / 4);
	corner_h = (shield_h / 4);
	chunk = new Shield();
	chunk->init(this->window, this->renderer, this->resource);
	chunk->getHitBox(&x, &temp, &iStep, &jStep);
	delete chunk;
	x = shield_step;
	double slope = ((double)corner_h / (double)corner_w);

	for (int k = 0; k < NUM_SHIELDS; k++)
	{
		for (int j = 0; j < shield_h; j += jStep)
		{
			y1 = (double)j + (double)(jStep / 2.0);
			for (int i = 0; i < shield_w; i += iStep)
			{
				x1 = (double)i + (double)(iStep / 2.0);
				skip = false;
				if ((i <= corner_w) && (j <= corner_h) && (((slope * x1) - corner_h)  * -1 >= y1)) {
					skip = true;
				}
				else if ((i >= shield_w - corner_w) && (j <= corner_h) && ((slope * (shield_w - x1)) - corner_h) * -1 >= y1) {
					skip = true;
				}
				else if ((i >= corner_w) && (i <= (shield_w - corner_w)) && (j >= (shield_h - corner_h))) {
					skip = true;
				}

				if (!skip)
				{
					chunk = new Shield();
					chunk->init(this->window, this->renderer, this->resource);
					chunk->setPosition((double)(x + i), (double)(y + j));
					this->shield.add((Sprite*)chunk);
				}
			}
		}
		x += (shield_step * 4);
	}
}

void SceneGame::drawFPS(double fps, int winW, int winH)
{
	// Write the fps
	char buffer[50];
	snprintf(buffer, 50, "fps: %3.2f", fps);	
	SDL_Color fg = { 255, 255, 255 };
	SDL_Surface* fpsSurface = TTF_RenderText_Solid(this->font, buffer, fg);	
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

	SDL_FreeSurface(fpsSurface);
	SDL_DestroyTexture(fpsTex);
}