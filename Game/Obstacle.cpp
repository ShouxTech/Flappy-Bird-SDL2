#include "Obstacle.h"
#include "Settings.h"
#include "EventEmitter.h"

Obstacle::Obstacle(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect dstRect) {
	this->Texture = texture;
	this->SrcRect = srcRect;
	this->DstRect = dstRect;

	this->X = dstRect.x;
	this->Y = dstRect.y;
}

Obstacle::~Obstacle() {
	SDL_DestroyTexture(this->Texture);
}

float Obstacle::GetX() {
	return this->X;
}

float Obstacle::GetY() {
	return this->Y;
}

SDL_Rect Obstacle::GetDstRect() {
	return this->DstRect;
}

bool Obstacle::IsOffScreen() {
	return this->OffScreen;
}

void Obstacle::Update() {
	if (this->OffScreen) return;

	if (this->X + this->DstRect.h < 0) {
		this->OffScreen = true;
		EventEmitter::Emit("ObstacleDestroyed");
	}

	this->X -= 5;

	// Update position.
	this->DstRect.x = this->X;
	this->DstRect.y = this->Y;
}

void Obstacle::Render(RenderWindow* renderWindow) {
	if (this->OffScreen) return;
	renderWindow->Render(this->Texture, this->SrcRect, this->DstRect);
}