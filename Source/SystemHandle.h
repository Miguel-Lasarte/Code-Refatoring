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
	~Window() noexcept {
		CloseWindow();
	}
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;
};

struct AudioDevice
{
	AudioDevice() {
		InitAudioDevice();
		if(!IsAudioDeviceReady()) {
			throw std::runtime_error("Failed to initialize audio device");
		}
	}
	~AudioDevice() noexcept{
		CloseAudioDevice();
	}
	AudioDevice(const AudioDevice&) = delete;
	AudioDevice& operator=(const AudioDevice&) = delete;
	AudioDevice(AudioDevice&&) = delete;
	AudioDevice& operator=(AudioDevice&&) = delete;
};
