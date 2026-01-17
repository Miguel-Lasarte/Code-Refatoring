#pragma once
#include "raylib.h"
#include <string_view>

struct Window {

	Window(int width, int height,std::string_view title ) {
		InitWindow(width, height, title.data());
		if(!IsWindowReady()) {
			throw std::runtime_error("Failed to initialize window");
		}
	}
	~Window() {
		CloseWindow();
	}
};

struct Audio
{
	Audio() {
		InitAudioDevice();
		if(!IsAudioDeviceReady()) {
			throw std::runtime_error("Failed to initialize audio device");
		}
	}
	~Audio() {
		CloseAudioDevice();
	}
};
