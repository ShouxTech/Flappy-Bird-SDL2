#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "RenderWindow.h"

RenderWindow::RenderWindow(const std::string& title, int width, int height) {
	this->Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (!this->Window) {
		std::cout << "Failed to create window.\n";
	}

	this->Renderer = SDL_CreateRenderer(this->Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!this->Window) {
		std::cout << "Failed to create renderer.\n";
	}
}

RenderWindow::~RenderWindow() {
	SDL_DestroyWindow(this->Window);
}

SDL_Texture* RenderWindow::LoadTexture(const std::string& filePath) {
	SDL_Texture* texture = IMG_LoadTexture(this->Renderer, filePath.c_str());
	if (!texture) {
		std::cout << "Failed to load texture: " << filePath << '\n';
	}

	return texture;
}

void RenderWindow::Clear() {
	SDL_RenderClear(this->Renderer);
}

void RenderWindow::Render(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect dstRect) {
	SDL_RenderCopy(this->Renderer, texture, &srcRect, &dstRect);
}

void RenderWindow::Display() {
	SDL_RenderPresent(this->Renderer);
}