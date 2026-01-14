#include "raylib.h"
#include "game.h"
#include "Constants.h"
#include "ResourceHandle.h"
#include <exception>

int main()
{

	InitWindow(GameConstants::Screen::WIDTH, GameConstants::Screen::HEIGHT, "SPACE INVADERS");

	SetTargetFPS(GameConstants::Screen::TARGET_FPS);

	InitAudioDevice();

	try {
		Game game;
		while (!WindowShouldClose())
		{

			try {
				game.Update();


				BeginDrawing();

				ClearBackground(BLACK);



				game.Render();

				EndDrawing();
			}
			catch (const std::exception& e) {
				TraceLog(LOG_ERROR, "Game loop error: %s", e.what());
			}
		}
	}
	catch (const std::exception& e) {
		TraceLog(LOG_ERROR, "Game error: %s", e.what());
	}

	CloseAudioDevice();

	CloseWindow();

	return 0;
}