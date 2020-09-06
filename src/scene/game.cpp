#include <iostream>
#include <algorithm>
#include "game.h"
#include "../utility.h"
#include "../sprite/bullet.h"
#include "../sprite/invader.h"
#include "../sprite/shield.h"
#include "../sprite/tank.h"

const double INVADER_SHOOT_DELAY_MIN = 700;
const int INVADER_SHOOT_RANDOM = 700;
const double DROP_TOTAL = 12.0;
const int MAX_COLS = 10; // duplicate data :(
const int MAX_ROWS = 5; // duplicate data :(

const std::string PATH_BLOOP = "assets/sound/bloop.ogg";
const std::string PATH_FONT = "assets/font/NovaSquareBoldOblique.ttf";
const std::string PATH_LASER = "assets/sound/laser.ogg";

const int FONT_SIZE = 14;

SceneGame::SceneGame() {
	this->quit = false;	
	this->joystick = NULL;
	this->pause = false;
	this->points = 0;
	this->dropDistance = 0;
	this->nextDirection = 0.0;
	this->isGameOver = false;
	this->left = this->right = false;
#ifdef DEBUG
	this->showFPS = true;
#else
	this->showFPS = false;
#endif
}

SceneGame::~SceneGame() {
	this->destroy();
}

int SceneGame::getNextState() {
	return STATE_MENU;
}

void SceneGame::loadMedia() {
	this->bloop = this->resource->GetAudio(PATH_BLOOP);
	this->laser = this->resource->GetAudio(PATH_LASER);
	this->font = resource->GetFont(PATH_FONT, FONT_SIZE);
	
	if (this->bloop == NULL || this->laser == NULL || this->font == NULL)
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
	this->loadMedia();
	this->tank.init(this->window, this->renderer, this->resource);
	this->invader.init(this->window, this->renderer, this->resource);
	this->invaderBullet.init(this->window, this->renderer, this->resource);
	this->tankBullet.init(this->window, this->renderer, this->resource);
	this->shield.init(this->window, this->renderer, this->resource);

	this->initGame();
}

void SceneGame::initGame()
{
	this->points = 0;
	this->isGameOver = false;
	this->pause = false;
	this->quit = false;
	this->left = false;
	this->right = false;
	this->dropDistance = 0.0;
	this->nextDirection = 0.0;
	this->invaderShootDelay = INVADER_SHOOT_DELAY_MIN + (double)(rand() % INVADER_SHOOT_RANDOM);

	if (this->tank.length() > 0)
		this->tank.destroy();
	this->initTank();

	if (this->invaderBullet.length() > 0)
		this->invaderBullet.destroy();

	if (this->tankBullet.length() > 0)
		this->tankBullet.destroy();

	if (this->invader.length() > 0)
		this->invader.destroy();
	this->initInvaders();

	int w, h, winW, winH;
	SDL_GetWindowSize(this->window, &winW, &winH);
	int x, y;
	this->tank[0]->getHitBox(&x, &y, &w, &h);
	if (this->shield.length() > 0)
		this->shield.destroy();
	this->initShields((int)(winH - (2.5 * h)));	
}

void SceneGame::destroy() {
	this->tank.destroy();
	this->invader.destroy();
	this->invaderBullet.destroy();
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
	int winW, winH;
	bool showGUI = this->pause || this->isGameOver || this->invader.length() == 0;
	bool left = false, right = false;
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
			case SDLK_a:
			case SDLK_LEFT:
				this->left = false;
				break;

			case SDLK_d:
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
			case SDLK_a:
			case SDLK_LEFT:
				this->left = true;
				break;

			case SDLK_d:
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

	SDL_GetWindowSize(this->window, &winW, &winH);
	

	if (!showGUI) {
		this->invader.removeInactive();
		this->invader.update(ms);
		this->invaderPostUpdate(ms);
		this->invader.collisionDetect(&this->tankBullet, true);		

		this->tank.update(ms);
		((Tank*)this->tank[0])->setInput(this->left, this->right);
		this->tank.collisionDetect(&this->invader, false);
		this->tank.collisionDetect(&this->invaderBullet, false);

		this->invaderBullet.removeInactive();
		this->invaderBullet.update(ms);

		this->tankBullet.removeInactive();	
		this->tankBullet.update(ms);

		this->shield.removeInactive();
		this->shield.update(ms);
		this->shield.collisionDetect(&this->tankBullet, true);
		this->shield.collisionDetect(&this->invader, false);
		this->shield.collisionDetect(&this->invaderBullet, true);
	}	
	
	if (showGUI) {
		// Draw a pause message box
		if (this->pause)
		{
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
		}
		if (this->isGameOver)
		{
			ImGui::NewFrame();
			ImGui::SetNextWindowPos(ImVec2((float)(winW / 3.0), (float)(winH / 3.0)), ImGuiCond_::ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2((float)(winW / 3.0), (float)(winH / 5.0)), ImGuiCond_::ImGuiCond_Once);
			ImGui::Begin("Game Over");
			ImGui::Text("Game Over.");
			ImGui::Text("Would you like to play again ? ");
			if (ImGui::Button("Yes")) {
				this->initGame();
			}
			if (ImGui::Button("No")) {
				this->quit = true;
			}
			ImGui::End();
			ImGui::EndFrame();
		}
		else if (this->invader.length() == 0)
		{
			ImGui::NewFrame();
			ImGui::SetNextWindowPos(ImVec2((float)(winW / 4.0), (float)(winH / 3.0)), ImGuiCond_::ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2((float)(winW / 2.0), (float)(winH / 6.0)), ImGuiCond_::ImGuiCond_Once);
			ImGui::Begin("Next Level");
			ImGui::Text("Congratulations!");
			ImGui::Text("You have destroyed a wave of the Great Machine Armada.");
			if (ImGui::Button("Next Level")) {
				int score = this->points; // preserve the score.
				this->initGame();
				this->points = score;
			}
			ImGui::End();
			ImGui::EndFrame();
		}
		ImGui::Render();

	}

	SDL_RenderClear(this->renderer);

	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = winW;
	r.h = winH;

	SDL_SetRenderDrawColor(this->renderer, 8, 8, 32, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(this->renderer, &r);
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	//if (showGUI) ImGui::Render();

	//Apply the image
	// SDL_RenderClear(this->renderer);

	this->invader.draw();
	this->invaderBullet.draw();
	this->tankBullet.draw();
	this->tank.draw();
	this->shield.draw();
	
	if (this->showFPS) {
		this->drawFPS(fps, winW, winH);
	}
	this->drawScore(winW, winH);

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
	if (this->tankBullet.length() == 0)
	{
		Tank* tank = ((Tank*)this->tank[0]);
		int turretX, turretY;
		tank->getTurrentPosition(&turretX, &turretY);
		Bullet* bullet = new Bullet();
		bullet->init(this->window, this->renderer, this->resource);
		bullet->setDirection(-1);
		int bulletX, bulletY, bulletW, bulletH;
		bullet->setPosition(0.0, 0.0);
		bullet->getHitBox(&bulletX, &bulletY, &bulletW, &bulletH);
		turretX -= (bulletW / 2);
		turretY -= bulletH;
		bullet->setPosition(turretX, turretY);
		this->tankBullet.add((Sprite*)bullet);
		Mix_PlayChannel(-1, this->bloop, 0);
	}
}

void SceneGame::initInvaders()
{
	this->invader.destroy();

	for (int j = 0; j < MAX_ROWS; ++j)
	{
		for (int i = 0; i < MAX_COLS; ++i)
		{
			Invader* invader = new Invader();
			invader->init(this->window, this->renderer, this->resource);
			invader->setParent(this);
			invader->setType(j, i);
			this->invader.add((Sprite*)invader);
		}
	}

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

void SceneGame::initTank()
{
	Tank* tank = new Tank();
	tank->init(this->window, this->renderer, this->resource);
	tank->setParent(this);
	this->tank.add(tank);

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
	fpsRect.y = winH - height - 1;
	fpsRect.w = width;
	fpsRect.h = height;
	SDL_RenderCopy(this->renderer, fpsTex, NULL, &fpsRect);	

	SDL_FreeSurface(fpsSurface);
	SDL_DestroyTexture(fpsTex);
}

void SceneGame::drawScore(int winW, int winH)
{
	// Write the fps
	char buffer[50];
	snprintf(buffer, 50, "Score: %d", this->points);
	SDL_Color fg = { 255, 255, 255 };
	SDL_Surface* scoreSurface = TTF_RenderText_Solid(this->font, buffer, fg);
	SDL_Texture* scoreTex = SDL_CreateTextureFromSurface(this->renderer, scoreSurface);
	int width;
	int height;
	SDL_QueryTexture(scoreTex, NULL, NULL, &width, &height);
	SDL_Rect fpsRect;
	fpsRect.x = winW - width - 1;
	fpsRect.y = 0;
	fpsRect.w = width;
	fpsRect.h = height;
	SDL_RenderCopy(this->renderer, scoreTex, NULL, &fpsRect);

	SDL_FreeSurface(scoreSurface);
	SDL_DestroyTexture(scoreTex);
}


void SceneGame::addPoints(int points)
{
	this->points += points;
}

double SceneGame::getInvaderSpeed()
{
	const int SPEED_PER_INVADER = 6; // pixels per second
	return 40 + (((MAX_COLS * MAX_ROWS) - this->invader.length()) * SPEED_PER_INVADER); 
}

void SceneGame::invaderPostUpdate(double ms)
{
	double dist = (this->getInvaderSpeed() * ms) / 1000.0;
	if (this->dropDistance > 0.0)
	{
		this->dropDistance -= dist;
		if (this->dropDistance <= 0.0)
		{
			this->dropDistance = 0.0;

			for (int i = 0; i < this->invader.length(); ++i)
			{
				Invader* enemy = (Invader*)this->invader[i];
				if (enemy->getActive() && enemy->getVisible())
				{
					enemy->setDirection(this->nextDirection, 0.0);
				}
			}
			this->nextDirection = 0.0;
		}
	} 
	else
	{
		int x, y, w, h, winW, winH;
		double dx, dy;
		SDL_GetWindowSize(this->window, &winW, &winH);
		for (int i = 0; i < this->invader.length(); ++i)
		{
			Invader* enemy = (Invader*)this->invader[i];
			if (enemy->getActive() && enemy->getVisible())
			{				
				enemy->getHitBox(&x, &y, &w, &h);
				enemy->getDirection(&dx, &dy);
				if (dx <= 0.0 && x <= 0)
				{
					for (int j = 0; j < this->invader.length(); ++j)
					{
						((Invader*) this->invader[j])->setDirection(0.0, 1.0);
					}
					this->dropDistance = DROP_TOTAL;
					this->nextDirection = 1.0;
					break;
				}
				else if (dx >= 0.0 && x >= (winW - w - 1))
				{
					for (int j = 0; j < this->invader.length(); ++j)
					{
						((Invader*)this->invader[j])->setDirection(0.0, 1.0);
					}
					this->dropDistance = DROP_TOTAL;
					this->nextDirection = -1.0;
					break;
				}
			}
		}
	}
	this->invaderShootDelay -= ms;
	if (this->invaderShootDelay <= 0.0)
	{
		this->invaderShootDelay = INVADER_SHOOT_DELAY_MIN + (double)(rand() % INVADER_SHOOT_RANDOM);
		// pick an invader.
		if (this->invader.length() > 0 && this->invaderBullet.length() == 0)
		{
			Invader* invader = ((Invader*)this->invader[rand() % this->invader.length()]);
			int turretX, turretY;
			invader->getTurrentPosition(&turretX, &turretY);
			turretY += 1;
			Bullet* bullet = new Bullet();
			bullet->init(this->window, this->renderer, this->resource);
			bullet->setDirection(1);
			int bulletX, bulletY, bulletW, bulletH;
			bullet->setPosition(0.0, 0.0);
			bullet->getHitBox(&bulletX, &bulletY, &bulletW, &bulletH);
			turretX -= (bulletW / 2);
			bullet->setPosition(turretX, turretY);
			this->invaderBullet.add((Sprite*)bullet);
			Mix_PlayChannel(-1, this->laser, 0);
		}
	}
}

void SceneGame::gameOver()
{
	this->isGameOver = true;
}