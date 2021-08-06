#pragma once
#include "RenderWindow.h"

class Character {
	private:
		float X;
		float Y;

		float XSpeed;
		float YSpeed;

		bool Dead;

		SDL_Texture* Texture;

		SDL_Rect SrcRect;
		SDL_Rect DstRect;
	public:
		Character(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect dstRect);
		~Character();

		void Jump();

		void Update();
		void Render(RenderWindow* renderWindow);
};