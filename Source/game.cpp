#include "game.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#pragma warning (push)
#pragma warning(disable : 26446)
//TODO : Alot of magic numbers
//TODO : No const correctness

Game::Game() : resources()
{
	gameState = State::STARTSCREEN;
	score = 0;
}

Player::Player(float screenWidth) : xPos(screenWidth / 2.0f) {
}

Star::Star(Vector2 pos, float size) : initPosition(pos), position(pos), size(size) {
}

Background::Background() {
	stars.reserve(GameConstants::Background::STAR_COUNT);

	for (int i = 0; i < GameConstants::Background::STAR_COUNT; ++i) {
		const Vector2 pos = {
			static_cast<float>(GetRandomValue(-GameConstants::Background::Stars::SPAWN_OFFSET, GetScreenWidth() + GameConstants::Background::Stars::SPAWN_OFFSET)),
			static_cast<float>(GetRandomValue(0,GetRenderHeight()))
		};
		const float size = static_cast<float>(GetRandomValue(GameConstants::Background::Stars::MIN_SIZE, GameConstants::Background::Stars::MAX_SIZE)) / GameConstants::Background::Stars::SIZE_DIVISOR;
		stars.emplace_back(pos, size);
	}
}

//TODO : Mixing Initialization and game logic
void Game::Start()
{
	// creating walls 
	float window_width = (float)GetScreenWidth();
	float window_height = (float)GetScreenHeight();
	float wall_distance = window_width / (wallCount + 1);
	for (int i = 0; i < wallCount; i++)
	{
		Wall newWalls;
		newWalls.position.y = window_height - GameConstants::Wall::Y_OFFSET;
		newWalls.position.x = wall_distance * (i + 1);

		Walls.push_back(newWalls);

	}

	//TODO : Remove two-step initialization
	//creating player
	player = Player(static_cast<float>(GetScreenWidth()));

	//creating aliens
	SpawnAliens();

	//TODO : Remove two-step initialization
	//creating background
	background = Background();

	//reset score
	score = 0;

	gameState = State::GAMEPLAY;

}

//TODO : Add error handling
void Game::End()
{
	//SAVE SCORE AND UPDATE SCOREBOARD
	Projectiles.clear();
	Walls.clear();
	Aliens.clear();
	newHighScore = CheckNewHighScore();
	gameState = State::ENDSCREEN;
}

void Game::Continue()
{
	SaveLeaderboard();
	gameState = State::STARTSCREEN;
}

//TODO : Too long function, break into smaller functions
void Game::Update()
{
	switch (gameState)
	{
	case State::STARTSCREEN:
		//Code 
		if (IsKeyReleased(KEY_SPACE))
		{
			Start();


		}

		break;
	case State::GAMEPLAY:
		//Code
		if (IsKeyReleased(KEY_Q))
		{
			End();
		}

		//Update Player
		player.Update();
		//TODO: Extract function
		//Update Aliens and Check if they are past player
		for (int i = 0; i < Aliens.size(); i++)
		{
			Aliens[i].Update();

			if (Aliens[i].position.y > GetScreenHeight() - GameConstants::Player::BASE_HEIGHT)
			{
				End();
			}
		}
		//TODO: Extract function
		//End game if player dies
		if (player.lives < 1)
		{
			End();
		}
		//TODO: Extract function
		//Spawn new aliens if aliens run out
		if (Aliens.size() < 1)
		{
			SpawnAliens();
		}

		//TODO: Extract function
		// Update background with offset
		playerPos = { player.GetXPos(), GameConstants::Player::BASE_HEIGHT };
		cornerPos = { 0, GameConstants::Player::BASE_HEIGHT };
		offset = CollisionSystem::CalculateLineLength(playerPos, cornerPos) * -1;
		background.Update(offset / GameConstants::Background::PARALLAX_SPEED);

		//TODO: Extract function
		//UPDATE PROJECTILE
		for (int i = 0; i < Projectiles.size(); i++)
		{
			Projectiles[i].Update();
		}
		//TODO: Extract function
		//UPDATE PROJECTILE
		for (int i = 0; i < Walls.size(); i++)
		{
			Walls[i].Update();
		}

		//CHECK ALL COLLISONS HERE
		//TODO : Breaking law of Demeter
		//TODO: Extract function
		for (int i = 0; i < Projectiles.size(); i++)
		{
			if (Projectiles[i].type == EntityType::PLAYER_PROJECTILE)
			{
				for (int a = 0; a < Aliens.size(); a++)
				{
					if (CollisionSystem::CheckCollision(Aliens[a].position, GameConstants::Alien::RADIUS, Projectiles[i].lineStart, Projectiles[i].lineEnd))
					{
						// Kill!
						std::cout << "Hit! \n";
						// Set them as inactive, will be killed later
						Projectiles[i].active = false;
						Aliens[a].active = false;
						score += GameConstants::Scoring::POINTS_PER_ALIEN;
					}
				}
			}

			//ENEMY PROJECTILES HERE
			//TODO: Extract function
			for (int j = 0; j < Projectiles.size(); j++)
			{
				if (Projectiles[j].type == EntityType::ENEMY_PROJECTILE)
				{
					if (CollisionSystem::CheckCollision({ player.GetXPos(), GetScreenHeight() - GameConstants::Player::BASE_HEIGHT }, GameConstants::Player::BASE_HEIGHT, Projectiles[j].lineStart, Projectiles[j].lineEnd))
					{
						std::cout << "dead!\n";
						Projectiles[j].active = false;
						player.lives -= 1;
					}
				}
			}


			for (int b = 0; b < Walls.size(); b++)
			{
				if (CollisionSystem::CheckCollision(Walls[b].position, GameConstants::Wall::RADIUS, Projectiles[i].lineStart, Projectiles[i].lineEnd))
				{
					// Kill!
					std::cout << "Hit! \n";
					// Set them as inactive, will be killed later
					Projectiles[i].active = false;
					Walls[b].health -= 1;
				}
			}
		}

		//MAKE PROJECTILE
		//TODO: Extract function
		if (IsKeyPressed(KEY_SPACE))
		{
			float window_height = static_cast<float>(GetScreenHeight());
			Projectile newProjectile;
			newProjectile.position.x = player.GetXPos();
			newProjectile.position.y = window_height - GameConstants::Player::Shooting::SPAWN_Y_OFFSET;

			newProjectile.type = EntityType::PLAYER_PROJECTILE;
			Projectiles.push_back(newProjectile);
		}

		//Aliens Shooting
		//TODO: Extract function
		shootTimer += 1;
		if (shootTimer > GameConstants::Alien::Shooting::INTERVAL_FRAMES) //once per second
		{
			int randomAlienIndex = 0;

			if (Aliens.size() > 1)
			{
				randomAlienIndex = rand() % Aliens.size();
			}

			Projectile newProjectile;
			newProjectile.position = Aliens[randomAlienIndex].position;
			newProjectile.position.y += GameConstants::Alien::Shooting::Y_OFFSET;
			newProjectile.speed = GameConstants::Alien::Shooting::PROJECTILE_SPEED;
			newProjectile.type = EntityType::ENEMY_PROJECTILE;
			Projectiles.push_back(newProjectile);
			shootTimer = 0;
		}

		// REMOVE INACTIVE/DEAD ENITITIES
		//TODO: Extract function
		for (int i = 0; i < Projectiles.size(); i++)
		{
			if (Projectiles[i].active == false)
			{
				Projectiles.erase(Projectiles.begin() + i);
				// Prevent the loop from skipping an instance because of index changes, since all insances after
				// the killed objects are moved down in index. This is the same for all loops with similar function
				i--;
			}
		}
		for (int i = 0; i < Aliens.size(); i++)
		{
			if (Aliens[i].active == false)
			{
				Aliens.erase(Aliens.begin() + i);
				i--;
			}
		}
		for (int i = 0; i < Walls.size(); i++)
		{
			if (Walls[i].active == false)
			{
				Walls.erase(Walls.begin() + i);
				i--;
			}
		}




		break;
		//TODO : Mixing game logic with UI logic
	case State::ENDSCREEN:
		//Code

		//Exit endscreen
		if (IsKeyReleased(KEY_ENTER) && !newHighScore)
		{
			Continue();
		}



		if (newHighScore)
		{
			using namespace GameConstants::UI::EndScreen::NameEntry;
			if (CheckCollisionPointRec(GetMousePosition(), { TEXTBOX_X,TEXTBOX_Y,TEXTBOX_WIDTH,TEXTBOX_HEIGHT })) mouseOnText = true;
			else mouseOnText = false;

			if (mouseOnText)
			{
				// Set the window's cursor to the I-Beam
				SetMouseCursor(MOUSE_CURSOR_IBEAM);

				// Get char pressed on the queue
				int key = GetCharPressed();

				// Check if more characters have been pressed on the same frame
				while (key > 0)
				{
					// NOTE: Only allow keys in range [32..125]
					if ((key >= 32) && (key <= 125) && (letterCount < GameConstants::UI::MAX_NAME_CHARS))
					{
						//TODO : Remove c-style string manipulation with char array
						name[letterCount] = (char)key;
						name[letterCount + 1] = '\0'; // Add null terminator at the end of the string.
						letterCount++;
					}

					key = GetCharPressed();  // Check next character in the queue
				}

				//Remove chars 
				if (IsKeyPressed(KEY_BACKSPACE))
				{
					letterCount--;
					if (letterCount < 0) letterCount = 0;
					name[letterCount] = '\0';
				}
			}
			else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

			if (mouseOnText)
			{
				framesCounter++;
			}
			else
			{
				framesCounter = 0;
			}

			// If the name is right legth and enter is pressed, exit screen by setting highscore to false and add 
			// name + score to scoreboard
			if (letterCount > 0 && letterCount < GameConstants::UI::MAX_NAME_CHARS && IsKeyReleased(KEY_ENTER))
			{
				std::string nameEntry(name);

				InsertNewHighScore(nameEntry);

				newHighScore = false;
			}


		}



		break;
	default:
		//SHOULD NOT HAPPEN
		break;
	}
}

//TODO : Too long function, break into smaller functions
void Game::Render()
{
	using namespace GameConstants::UI;
	switch (gameState)
	{
	case State::STARTSCREEN:
		//Code
		DrawText("SPACE INVADERS", StartScreen::TITLE_X, StartScreen::TITLE_Y, StartScreen::TITLE_SIZE, YELLOW);

		DrawText("PRESS SPACE TO BEGIN", StartScreen::PROMPT_X, StartScreen::PROMPT_Y, StartScreen::PROMPT_SIZE, YELLOW);


		break;
	case State::GAMEPLAY:
		//Code


		//background render LEAVE THIS AT TOP
		background.Render();

		//DrawText("GAMEPLAY", 50, 30, 40, YELLOW);
		DrawText(TextFormat("Score: %i", score), HUD::SCORE_X, HUD::SCORE_Y, HUD::TEXT_SIZE, YELLOW);
		DrawText(TextFormat("Lives: %i", player.lives), HUD::LIVES_X, HUD::LIVES_Y, HUD::TEXT_SIZE, YELLOW);

		//player rendering 
		player.Render(resources.GetShipTexture(player.GetActiveTextureIndex()));

		//projectile rendering
		for (int i = 0; i < Projectiles.size(); i++)
		{
			Projectiles[i].Render(resources.GetProjectileTexture());
		}

		// wall rendering 
		for (int i = 0; i < Walls.size(); i++)
		{
			Walls[i].Render(resources.GetWallTexture());
		}

		//alien rendering  
		for (int i = 0; i < Aliens.size(); i++)
		{
			Aliens[i].Render(resources.GetAlienTexture());
		}



		break;
	case State::ENDSCREEN:
		//Code
		//DrawText("END", 50, 50, 40, YELLOW);



		if (newHighScore)
		{
			DrawText("NEW HIGHSCORE!", EndScreen::HIGHSCORE_X, EndScreen::HIGHSCORE_Y, EndScreen::HIGHSCORE_SIZE, YELLOW);


			using namespace GameConstants::UI::EndScreen::NameEntry;
			// BELOW CODE IS FOR NAME INPUT RENDER
			DrawText("PLACE MOUSE OVER INPUT BOX!", TEXTBOX_X, PROMPT_Y, PROMPT_SIZE, YELLOW);

			DrawRectangle(TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT, LIGHTGRAY);
			if (mouseOnText)
			{
				// HOVER CONFIRMIATION
				DrawRectangleLines(TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT, RED);
			}
			else
			{
				DrawRectangleLines(TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT, DARKGRAY);
			}

			//Draw the name being typed out
			DrawText(name, TEXTBOX_X + TEXT_X_OFFSET, TEXTBOX_Y + TEXT_Y_OFFSET, TEXT_SIZE, MAROON);

			//Draw the text explaining how many characters are used
			DrawText(TextFormat("INPUT CHARS: %i/%i", letterCount, MAX_NAME_LENGTH), TEXTBOX_X, COUNT_Y, PROMPT_SIZE, YELLOW);

			if (mouseOnText)
			{
				if (letterCount < MAX_NAME_CHARS)
				{
					// Draw blinking underscore char
					if (((framesCounter / GameConstants::UI::TEXT_BLINK_INTERVAL) % 2) == 0)
					{
						DrawText("_", TEXTBOX_X + TEXT_X_OFFSET + MeasureText(name, TEXT_SIZE), TEXTBOX_Y + TEXT_Y_OFFSET, TEXT_SIZE, MAROON);
					}

				}
				else
				{
					//Name needs to be shorter
					DrawText("Press BACKSPACE to delete chars...", TEXTBOX_X, BACKSPACE_PROMPT_Y, PROMPT_SIZE, YELLOW);
				}

			}

			// Explain how to continue when name is input
			if (letterCount > 0 && letterCount < MAX_NAME_CHARS)
			{
				DrawText("PRESS ENTER TO CONTINUE", TEXTBOX_X, CONTINUE_PROMT_Y, EndScreen::CONTINUE_SIZE, YELLOW);
			}

		}
		else {
			// If no highscore or name is entered, show scoreboard and call it a day
			DrawText("PRESS ENTER TO CONTINUE", EndScreen::HIGHSCORE_X, EndScreen::CONTINUE_Y, EndScreen::CONTINUE_SIZE, YELLOW);

			DrawText("LEADERBOARD", Leaderboard::TITLE_X, Leaderboard::TITLE_Y, HUD::TEXT_SIZE, YELLOW);

			for (int i = 0; i < Leaderboard.size(); i++)
			{
				char* tempNameDisplay = Leaderboard[i].name.data();
				DrawText(tempNameDisplay, Leaderboard::NAME_X, Leaderboard::START_Y + (i * Leaderboard::ROW_HEIGHT), HUD::TEXT_SIZE, YELLOW);
				DrawText(TextFormat("%i", Leaderboard[i].score), Leaderboard::SCORE_X, Leaderboard::START_Y + (i * Leaderboard::ROW_HEIGHT), HUD::TEXT_SIZE, YELLOW);
			}
		}



		break;
	default:
		//SHOULD NOT HAPPEN
		break;
	}
}

void Game::SpawnAliens()
{
	using namespace GameConstants::Formation;
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			Alien newAlien = Alien();
			newAlien.active = true;
			newAlien.position.x = static_cast<float>(START_X + OFFSET_X + (col * SPACING));
			newAlien.position.y = static_cast<float>(START_Y + (row * SPACING));
			Aliens.push_back(newAlien);
			// TODO: Remove debug print statements
			std::cout << "Find Alien -X:" << newAlien.position.x << std::endl;
			std::cout << "Find Alien -Y:" << newAlien.position.y << std::endl;
		}
	}

}

bool Game::CheckNewHighScore()
{
	//TODO : Hardcoded leaderboard size
	//TODO : Should return false is leaderboard is empty
	if (score > Leaderboard[GameConstants::UI::LEADERBOARD_SIZE].score)
	{
		return true;
	}

	return false;
}

void Game::InsertNewHighScore(std::string playerName)
{
	PlayerData newData;
	newData.name = playerName;
	newData.score = score;

	//TODO : use a algorithm for the manual loop
	for (size_t i = 0; i < Leaderboard.size(); i++)
	{
		if (newData.score > Leaderboard[i].score)
		{

			Leaderboard.insert(Leaderboard.begin() + i, newData);

			Leaderboard.pop_back();

			i = Leaderboard.size();

		}
	}
}
\
//TODO : Incomplete code
void Game::LoadLeaderboard()
{
	// CLEAR LEADERBOARD

	// OPEN FILE

	// READ DATA

	// WRITE DATA ONTO LEADERBOARD

	//CLOSE FILE
}

//TODO : Incomplete code
//TODO : Add error handling for file operations
void Game::SaveLeaderboard()
{
	// SAVE LEADERBOARD AS ARRAY

	// OPEN FILE
	std::fstream file;

	//TODO : Open but unused
	file.open("Leaderboard");

	if (!file)
	{
		std::cout << "file not found \n";

	}
	else
	{
		std::cout << "file found \n";
	}
	// CLEAR FILE

	// WRITE ARRAY DATA INTO FILE

	// CLOSE FILE
}

void Player::Update()
{
	using namespace GameConstants::Player;
	direction = 0;
	if (IsKeyDown(KEY_LEFT)) direction--;

	if (IsKeyDown(KEY_RIGHT)) direction++;

	xPos += SPEED * static_cast<float>(direction);

	const float minX = RADIUS;
	const float maxX = static_cast<float>(GetScreenWidth()) - RADIUS;

	if (xPos < minX) xPos = minX;
	else if (xPos > maxX) xPos = maxX;

	timer += GetFrameTime();
	if (timer >= Animation::INTERVAL)
	{
		activeTexture = (activeTexture + 1) % Rendering::TEXTURE_COUNT;
		timer = 0.f;
	}
}

void Player::Render(Texture2D resources)
{
	using namespace GameConstants::Player;
	float window_height = static_cast<float>(GetScreenHeight());

	DrawTexturePro(resources,
		{
			0,
			0,
			Rendering::SOURCE_SIZE,
			Rendering::SOURCE_SIZE,
		},
		{
			xPos, window_height - BASE_HEIGHT,
			Rendering::TEXTURE_SIZE,
			Rendering::TEXTURE_SIZE,
		}, { Rendering::TEXTURE_OFFSET , Rendering::TEXTURE_OFFSET },
		0,
		WHITE);
}

void Projectile::Update()
{
	using namespace GameConstants::Projectile;
	position.y -= speed;

	// UPDATE LINE POSITION
	lineStart.y = position.y - LENGTH;
	lineEnd.y = position.y + LENGTH;

	lineStart.x = position.x;
	lineEnd.x = position.x;

	if (position.y < 0 || position.y > OUT_OF_BOUNDS)
	{
		active = false;
	}
}

// TODO : Projectile dont need to know about texture details
void Projectile::Render(Texture2D texture)
{
	using namespace GameConstants::Projectile::Rendering;
	//DrawCircle((int)position.x, (int)position.y, 10, RED);
	DrawTexturePro(texture,
		{
			0,
			0,
			SOURCE_SIZE,
			SOURCE_SIZE,
		},
		{
			position.x,
			position.y,
			TEXTURE_SIZE,
			TEXTURE_SIZE,
		}, { TEXTURE_OFFSET , TEXTURE_OFFSET },
		0,
		WHITE);
}

// TODO : Wall dont need to know about texture details
void Wall::Render(Texture2D texture)
{
	using namespace GameConstants::Wall::Rendering;
	DrawTexturePro(texture,
		{
			0,
			0,
			SOURCE_SIZE,
			SOURCE_SIZE,
		},
		{
			position.x,
			position.y,
			TEXTURE_SIZE,
			TEXTURE_SIZE,
		}, { TEXTURE_OFFSET , TEXTURE_OFFSET },
		0,
		WHITE);


	// Cast float to int to resolve C4244 warning
	DrawText(
		TextFormat("%i", health),
		static_cast<int>(position.x - HealthDisplay::TEXT_OFFSET_X),
		static_cast<int>(position.y + HealthDisplay::TEXT_OFFSET_Y),
		HealthDisplay::TEXT_SIZE,
		RED
	);
}

void Wall::Update()
{

	// set walls as inactive when out of health
	if (health < 1)
	{
		active = false;
	}


}

void Alien::Update()
{
	using namespace GameConstants::Alien;

	if (moveRight)
	{
		position.x += SPEED;

		if (position.x >= GetScreenWidth())
		{
			moveRight = false;
			position.y += DROP_DISTANCE;
		}
	}
	else
	{
		position.x -= SPEED;

		if (position.x <= 0)
		{
			moveRight = true;
			position.y += DROP_DISTANCE;
		}
	}
}

// TODO : Alien dont need to know about texture details
void Alien::Render(Texture2D texture)
{
	using namespace GameConstants::Alien::Rendering;
	//DrawRectangle((int)position.x - 25, (int)position.y, 30, 30, RED);
	//DrawCircle((int)position.x, (int)position.y, radius, GREEN);



	DrawTexturePro(texture,
		{
			0,
			0,
			SOURCE_SIZE,
			SOURCE_SIZE,
		},
		{
			position.x,
			position.y,
			TEXTURE_SIZE,
			TEXTURE_SIZE,
		}, { TEXTURE_OFFSET , TEXTURE_OFFSET },
		0,
		WHITE);
}

//BACKGROUND
void Star::Update(float offset)
{
	position.x = initPosition.x + offset;
	position.y = initPosition.y;

}

void Star::Render() const
{
	DrawCircle(
		static_cast<int>(position.x),
		static_cast<int>(position.y),
		size,
		color
	);
}

void Background::Update(float offset)
{
	for(auto& star : stars)
	{
		star.Update(offset);
	}

}

void Background::Render() const
{
	for (const auto& star : stars)
	{
		star.Render();
	}
}

#pragma warning (pop)












//TODO : Legacy code removal

/*LEGACY CODE
	// our objective is to calculate the distance between the closest point of the line to the centre of the circle,
	// and determine if it is shorter than the radius.

	// we can imagine the edges of the line and circle centre to form a triangle. calculating the height of the
	// triangle will give us the distance, if the line serves as the base

	// simplify variables
	Vector2 A = lineStart;
	Vector2 B = lineEnd;
	Vector2 C = circlePos;

	// calculate area using determinant method

	float triangle_area = fabsf(A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)) / 2;


	// Caculate vectors AB to calculate base length
	Vector2 AB;
	AB.x = B.x - A.x;
	AB.y = B.y - A.y;

	//get the base length
	float trangle_base_length = (float)sqrt(pow(AB.x, 2) + pow(AB.y, 2));

	// we double the area to turn in into a rectangle, and then divide the base length to get the height.
	float triangle_height = (triangle_area * 2 ) / trangle_base_length;

	std::cout << triangle_area << "\n";

	if (triangle_height < circleRadius)
	{
		return true;
	}
	else
	{
		return false;
	}


	*/

