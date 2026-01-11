/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*b
*   Copyright (c) 2013-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "game.h"
#include "Constants.h"
#include "ResourceHandle.h"
#include <exception>




//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{

    InitWindow(GameConstants::Screen::WIDTH, GameConstants::Screen::HEIGHT, "SPACE INVADERS");

    SetTargetFPS(GameConstants::Screen::TARGET_FPS);               // Set our game to run at 60 frames-per-second

    InitAudioDevice();
    
    try {
		Game game;
        while (!WindowShouldClose())    // Detect window close button or ESC key
        {
            

            game.Update();


            BeginDrawing();

            ClearBackground(BLACK);



            game.Render();

            EndDrawing();
           
        }
    }
    catch (const std::exception& e) {
		TraceLog(LOG_ERROR,"Game error: %s", e.what());
	}

    CloseAudioDevice();
  
    CloseWindow();       

    return 0;
}