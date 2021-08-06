#pragma once

#include <string>
#include <SDL.h>
#include <SDL_image.h>

class RenderWindow {
	private:
		SDL_Window* Window;
		SDL_Renderer* Renderer;
	public:
		RenderWindow(const std::string& title, int width, int height);
		~RenderWindow();

		SDL_Texture* LoadTexture(const std::string& filePath);

		void Clear();
		void Render(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect dstRect);
		void Display();
};