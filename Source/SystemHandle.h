#pragma once
#include "raylib.h"

struct Window {

	Window(int width, int height, const char* title) {
		InitWindow(width, height, title);
	}
	~Window() {
		CloseWindow();
	}
};

struct Audio
{
	Audio() {
		InitAudioDevice();
	}
	~Audio() {
		CloseAudioDevice();
	}
};
