#pragma once
#include "raylib.h"
#include <string_view>
#include <stdexcept>

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
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;
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
	Audio(const Audio&) = delete;
	Audio& operator=(const Audio&) = delete;
	Audio(Audio&&) = delete;
	Audio& operator=(Audio&&) = delete;
};
