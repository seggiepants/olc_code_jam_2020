#include <iostream>
#include <algorithm>
#include "game.h"
#include "../utility.h"

const int MOVE_SPEED = 400; // pixels per second.
const int FONT_DEFAULT = 0;
const int SOUND_BLOOP = 0;
const int TEXTURE_TANK = 0;

const std::string PATH_BLOOP = "assets/sound/bloop.ogg";
const std::string PATH_TANK = "assets/image/tank.png";
const std::string PATH_TANK_BULLET = "assets/image/tank_bullet.png";
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
	this->tankBullet = resource->GetImage(this->renderer, PATH_TANK_BULLET);

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
}

void SceneGame::destroy() {

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
	int bulletW, bulletH;

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
				if (this->bulletActive == false && !showGUI)
				{
					SDL_QueryTexture(this->tank, NULL, NULL, &w, &h);
					SDL_QueryTexture(this->tankBullet, NULL, NULL, &bulletW, &bulletH);
					this->bulletX = this->x + ((w - bulletW) / 2); 
					this->bulletY = this->y - bulletH - 1;
					this->bulletActive = true;
					Mix_PlayChannel(-1, this->bloop, 0);
				}
				break;

			case SDLK_p:
				this->pause = !this->pause;
				break;

			default:
				std::cout << "[UP] Other Key " << e.key.keysym.sym << std::endl;
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
				std::cout << "[DOWN] Other Key " << e.key.keysym.sym << std::endl;
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
				std::cout << "Joystick button down. Button #" << (int)e.jbutton.button << std::endl;
			}
		}
		else if (e.type == SDL_JOYBUTTONUP) {
			if (e.jbutton.which == 0)
			{
				if (e.jbutton.button == 1) {
					this->quit = true;
				}
				else {
					std::cout << "Joystick button up. Button #" << (int)e.jbutton.button << std::endl;
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

		if (this->bulletActive) {
			this->bulletY -= step;
			SDL_QueryTexture(this->tankBullet, NULL, NULL, &bulletW, &bulletH);
			if (this->bulletY <= bulletH * -1) {
				this->bulletActive = false;
			}
		}
	}	
	
	if (showGUI) {
		// Draw a pause message box
		ImGui::NewFrame();
		ImGui::SetNextWindowPos(ImVec2(winW / 3.0, winH / 3.0), ImGuiCond_::ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(winW / 3.0, winH / 8.0), ImGuiCond_::ImGuiCond_Once);
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

	
	SDL_SetRenderDrawColor(this->renderer, 64, 64, 255,SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(this->renderer, &fillR);
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	//if (showGUI) ImGui::Render();
	
	//Apply the image
	// SDL_RenderClear(this->renderer);

	if (this->bulletActive) {
		r.x = (int)this->bulletX;
		r.y = (int)this->bulletY;
		SDL_QueryTexture(this->tankBullet, NULL, NULL, &bulletW, &bulletH);
		r.w = bulletW;
		r.h = bulletH;
		SDL_RenderCopy(this->renderer, this->tankBullet, NULL, &r);
	}

	r.x = (int)this->x;
	r.y = (int)this->y;
	r.w = w;
	r.h = h;

	SDL_RenderCopy(this->renderer, this->tank, NULL, &r);

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
	// std::cout << "Rect x: " << fpsRect.x << " y: " << fpsRect.y << " w: " << fpsRect.w << " h: " << fpsRect.h << std::endl;

	SDL_FreeSurface(fpsSurface);
	SDL_DestroyTexture(fpsTex);


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