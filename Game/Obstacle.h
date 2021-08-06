#pragma once

#include "RenderWindow.h"

class Obstacle {
	private:
		float X;
		float Y;

		bool OffScreen;

		SDL_Texture* Texture;

		SDL_Rect SrcRect;
		SDL_Rect DstRect;
	public:
		Obstacle(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect dstRect);
		~Obstacle();

		float GetX();
		float GetY();
		SDL_Rect GetDstRect();
		bool IsOffScreen();

		void Update();
		void Render(RenderWindow* renderWindow);
};