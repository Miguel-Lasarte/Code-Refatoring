#include "raylib.h"
#include "game.h"
#include "Constants.h"
#include "ResourceHandle.h"
#include <exception>

int main()
{
	try {
		InitWindow(GameConstants::Screen::WIDTH, GameConstants::Screen::HEIGHT, "SPACE INVADERS");
		SetTargetFPS(GameConstants::Screen::TARGET_FPS);
		InitAudioDevice();

		auto maybeGame = Game::TryCreate();
		if (!maybeGame) {
			TraceLog(LOG_ERROR, "Failed to create game instance");
			CloseAudioDevice();
			CloseWindow();
			return -1;
		}

		Game& game = *maybeGame;

		try {
			while (!WindowShouldClose())
			{
				game.Update();

				BeginDrawing();
				ClearBackground(BLACK);
				game.Render();
				EndDrawing();
			}
		}
		catch (const std::exception& e) {
			TraceLog(LOG_ERROR, "Runtime error during game loop: %s", e.what());
		}

		CloseAudioDevice();
		CloseWindow();

		return 0;
	}
	catch (const std::exception& e) {
		TraceLog(LOG_FATAL, "Fatal error: %s", e.what());

		if (IsAudioDeviceReady()) {
			CloseAudioDevice();
		}
		if (IsWindowReady()) {
			CloseWindow();
		}

		return -1;
	}
	catch (...) {
		TraceLog(LOG_FATAL, "Unknown fatal error occurred");

		if (IsAudioDeviceReady()) {
			CloseAudioDevice();
		}
		if (IsWindowReady()) {
			CloseWindow();
		}

		return -1;
	}
}