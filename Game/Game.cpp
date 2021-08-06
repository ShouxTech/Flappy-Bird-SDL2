#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "RenderWindow.h"
#include "Settings.h"
#include "Character.h"
#include "Obstacle.h"
#include "EventEmitter.h"

#undef main

std::vector<SDL_Keycode> pressedKeys;

Character* character;
std::vector<Obstacle*> obstacles;

bool isPaused = false;

float getElapsedTime() {
	return SDL_GetTicks() * 0.001;
}

int getRandomNumber(int min, int max) {
	int range = max - min + 1;
	int randomNumber = rand() % range + min;
	return randomNumber;
}

void keyPressed(SDL_Event event) {
	switch (event.key.keysym.sym) { // Pressed and released key.
		case SDLK_SPACE: {
			if (!character || isPaused) return;
			character->Jump();
			break;
		}
		case SDLK_ESCAPE: {
			isPaused = !isPaused;
			break;
		}
	}
}

void keyHandler(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		auto foundIndex = std::find(pressedKeys.begin(), pressedKeys.end(), event.key.keysym.sym);
		if (foundIndex == pressedKeys.end()) { // If the key is not being held.
			keyPressed(event);
		}

		pressedKeys.push_back(event.key.keysym.sym);
	} else if (event.type == SDL_KEYUP) {
		auto foundIndex = std::find(pressedKeys.begin(), pressedKeys.end(), event.key.keysym.sym);
		if (foundIndex != pressedKeys.end()) { // If the key is being held.
			pressedKeys.erase(foundIndex); // Remove it from the being held/pressed list.
		}
	}
}

void renderBackground(RenderWindow* renderWindow, SDL_Texture* backgroundTexture) {
	SDL_Rect srcRect; // Original part of the texture that we want to draw.
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 1280;
	srcRect.h = 720;

	SDL_Rect dstRect; // How to place it on the renderer.
	dstRect.x = 0;
	dstRect.y = 0;
	dstRect.w = 1280;
	dstRect.h = 720;

	renderWindow->Render(backgroundTexture, srcRect, dstRect);
}

void createCharacter(RenderWindow* renderWindow) {
	SDL_Texture* characterTexture = renderWindow->LoadTexture("Resources/Images/Character.png");
	int originalWidth = 500;
	int originalHeight = 500;
	int dstWidth = 70;
	int dstHeight = 70;

	SDL_Rect srcRect; // Original part of the texture that we want to draw.
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = originalWidth;
	srcRect.h = originalHeight;

	SDL_Rect dstRect; // How to place it on the renderer.
	dstRect.x = (Settings::WindowWidth / 2) - (dstWidth / 2);
	dstRect.y = (Settings::WindowHeight / 2) - (dstHeight / 2);
	dstRect.w = dstWidth;
	dstRect.h = dstHeight;

	character = new Character(
		characterTexture, srcRect, dstRect
	);
}

void createObstaclePair(RenderWindow* renderWindow, SDL_Rect srcRect, float x) {
	int topObstacleHeight;

	for (int i = 0; i < 2; i++) {
		SDL_Texture* obstacleTexture = renderWindow->LoadTexture("Resources/Images/Obstacle.png");

		int dstWidth = 65;
		int dstHeight;

		std::function<void()> generateDstHeight = [&]() {
			dstHeight = getRandomNumber(10, 450);

			if (i == 0) {
				topObstacleHeight = dstHeight;
			} else {
				if (Settings::WindowHeight - (dstHeight + topObstacleHeight) < 170) {
					generateDstHeight();
				}
			}
		};

		generateDstHeight();

		SDL_Rect dstRect; // How to place it on the renderer.
		dstRect.x = x;
		dstRect.y = i == 1 ? Settings::WindowHeight - dstHeight : 0;
		dstRect.w = dstWidth;
		dstRect.h = dstHeight;

		Obstacle* obstacle = new Obstacle(
			obstacleTexture, srcRect, dstRect
		);

		obstacles.push_back(obstacle);
	}
}

void createObstacles(RenderWindow* renderWindow, int amount) {
	int originalWidth = 200;
	int originalHeight = 350;

	SDL_Rect srcRect; // Original part of the texture that we want to draw.
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = originalWidth;
	srcRect.h = originalHeight;

	for (int i = 0; i < amount; i++) {
		float x;
		if (obstacles.size() == 0) {
			x = Settings::WindowWidth;
		} else {
			x = obstacles.back()->GetX() + Settings::ObstacleSpacing;
		}

		createObstaclePair(renderWindow, srcRect, x);
	}
}

int main() {
	srand(time(0));

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Failed to init SDL video.\n";
		system("pause");
		return 0;
	}

	if (!IMG_Init(IMG_INIT_PNG)) {
		std::cout << "Failed to init IMG png.\n";
		system("pause");
		return 0;
	}

	RenderWindow* renderWindow = new RenderWindow("Game", Settings::WindowWidth, Settings::WindowHeight); // Creating the render window the heap so it doesn't get destroyed when it's out of scope.

	SDL_Texture* backgroundTexture = renderWindow->LoadTexture("Resources/Images/Background.png");

	createCharacter(renderWindow);
	createObstacles(renderWindow, Settings::AmountOfStartingObstacles);

	SDL_Event event;

	bool isRunning = true;

	int numberOfDestroyedObstacles;

	float currentTime = getElapsedTime();
	const float timeStep = 0.01;
	float accumulator = 0;

	EventEmitter::On("CharacterDied", [&]() {
		character->~Character(); // 'delete character' doesn't work.
		pressedKeys.clear();
		for (Obstacle* obstacle : obstacles) {
			obstacle->~Obstacle();
		}
		obstacles.clear();
		createCharacter(renderWindow);
		createObstacles(renderWindow, Settings::AmountOfStartingObstacles);
	});

	EventEmitter::On("ObstacleDestroyed", [&]() {
		for (Obstacle* obstacle : obstacles) {
			if (obstacle->IsOffScreen()) {
				auto index = std::find(obstacles.begin(), obstacles.end(), obstacle);
				if (index != obstacles.end()) {
					obstacles.erase(index);
					obstacle->~Obstacle(); // 'delete obstacle' doesn't work.
					numberOfDestroyedObstacles++;
					if (numberOfDestroyedObstacles % 2 == 0) { // If a pair of obstacles has been destroyed.
						createObstacles(renderWindow, 1); // Create a new pair of obstacles.
					}
					break;
				}
			}
		}
	});

	while (isRunning) {
		float newTime = getElapsedTime();
		float deltaTime = newTime - currentTime;

		currentTime = newTime;

		accumulator += deltaTime;

		while (accumulator >= timeStep) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					isRunning = false;
				} else if (event.type == SDL_KEYDOWN) {
					keyHandler(event);
				} else if (event.type == SDL_KEYUP) {
					keyHandler(event);
				}
			}
			accumulator -= timeStep;
		}

		if (isPaused) continue;

		renderWindow->Clear();

		renderBackground(renderWindow, backgroundTexture);

		character->Update();
		character->Render(renderWindow);

		for (Obstacle* obstacle : obstacles) {
			obstacle->Update();
			obstacle->Render(renderWindow);
		}

		renderWindow->Display();
	}

	delete character;
	for (Obstacle* obstacle : obstacles) {
		delete obstacle;
	}
	delete renderWindow;
	SDL_Quit();
}