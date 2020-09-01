#include <iostream>
#include <algorithm>
#include "menu.h"
#include "../utility.h"

const int TEXTURE_BG = 0;

SceneMenu::SceneMenu() {
	this->quit = false;
	this->nextState = STATE_EXIT;
}

SceneMenu::~SceneMenu() {
	this->destroy();
}

void SceneMenu::loadMedia() {

	SDL_Texture* texture = loadTexture(this->renderer, "assets/image/title.png");
	if (texture == NULL)
	{
		std::cout << "Failed to load default image!" << std::endl;
		this->quit = true;
	}
	this->texture[TEXTURE_BG] = texture;
}

void SceneMenu::init(SDL_Window* window, SDL_Renderer* renderer) {
	this->window = window;
	this->renderer = renderer;
	this->quit = false;
	this->texture.clear();
	this->loadMedia();
	this->nextState = STATE_EXIT;
}

int SceneMenu::getNextState() {
	return this->nextState;
}

void SceneMenu::destroy() {

	for (const auto &element : this->texture) {
		SDL_DestroyTexture(element.second);
	}
}

void SceneMenu::update(double ms) {
	SDL_Event e;

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
			}

			//Select surfaces based on key press
			switch (e.key.keysym.sym)
			{

			case SDLK_ESCAPE:
				this->quit = true;
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

			default:
				std::cout << "[DOWN] Other Key " << e.key.keysym.sym << std::endl;
				break;
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

	SDL_Rect r;
	SDL_Texture* bg = this->texture[TEXTURE_BG];
	int w, h, winW, winH;
	SDL_QueryTexture(bg, NULL, NULL, &w, &h);
	SDL_GetWindowSize(this->window, &winW, &winH);

	SDL_GetWindowSize(this->window, &winW, &winH);
	io.DisplaySize = ImVec2((float)w, (float)h);
	
	ImGui::NewFrame();
	ImGui::Begin("OLC Game Jam 2020");
	// ImGui::Text("OLC Game Jam 2020");
	if (ImGui::Button("Play")) {
		this->nextState = STATE_PLAY;
		this->quit = true;
	}
	if (ImGui::Button("Exit")) {
		this->nextState = STATE_EXIT;
		this->quit = true;
	}
	ImGui::End();
	ImGui::EndFrame();
	ImGui::Render();

	SDL_RenderClear(this->renderer);

	SDL_Rect fillR;
	fillR.x = 0;
	fillR.y = 0;
	fillR.w = winW;
	fillR.h = winH;


	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	r.x = 0;
	r.y = 0;
	r.w = winW;
	r.h = winH;

	SDL_RenderCopy(this->renderer, bg, NULL, &r);

	// Rendering		
	ImGuiSDL::Render(ImGui::GetDrawData());

	//Update the surface
	SDL_RenderPresent(this->renderer);
}

bool SceneMenu::running() {
	return !this->quit;
}