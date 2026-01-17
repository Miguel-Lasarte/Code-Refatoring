#include "raylib.h"
#include "game.h"
#include "Constants.h"
#include "ResourceHandle.h"
#include <exception>
#include "SystemHandle.h"

int main()
{
	try {
		Window window(
			GameConstants::Screen::WIDTH,
			GameConstants::Screen::HEIGHT,
			"SPACE INVADERS"
		);
		SetTargetFPS(GameConstants::Screen::TARGET_FPS);
		Audio audio;
		Game game;
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
		TraceLog(LOG_ERROR, "Fatal error: %s", e.what());
		return -1;
	}
	catch (...) {
		TraceLog(LOG_ERROR, "Unknown error occurred");

		return -1;
	}
}