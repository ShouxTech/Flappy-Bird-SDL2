#include <vector>
#include "Character.h"
#include "Obstacle.h"
#include "Settings.h"
#include "EventEmitter.h"

extern std::vector<Obstacle*> obstacles; // Pretty sure this isn't good practice.

Character::Character(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect dstRect) {
	this->Texture = texture;
	this->SrcRect = srcRect;
	this->DstRect = dstRect;

	this->X = dstRect.x;
	this->Y = dstRect.y;

	this->XSpeed = 0;
	this->YSpeed = 0;

	this->Dead = false;
}

Character::~Character() {
	SDL_DestroyTexture(this->Texture);
}

void Character::Jump() {
	this->YSpeed = -Settings::JumpHeight;
}

void Character::Update() {
	if (this->Dead) return;

	if (this->Y + this->DstRect.h > Settings::WindowHeight || this->Y < 0) {
		this->Dead = true;
		EventEmitter::Emit("CharacterDied");
	}

	for (Obstacle* obstacle : obstacles) {
		if (this->X + this->DstRect.w > obstacle->GetX()
			&& this->Y < obstacle->GetY() + obstacle->GetDstRect().h
			&& this->X < obstacle->GetX() + obstacle->GetDstRect().w
			&& this->Y + this->DstRect.h > obstacle->GetY()
		) {
			this->Dead = true;
			EventEmitter::Emit("CharacterDied");
		}
	}

	this->YSpeed += Settings::Gravity;
	this->Y += YSpeed;

	// Update position.
	this->DstRect.x = this->X;
	this->DstRect.y = this->Y;
}

void Character::Render(RenderWindow* renderWindow) {
	if (this->Dead) return;
	renderWindow->Render(this->Texture, this->SrcRect, this->DstRect);
}