#include "game.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#pragma warning (push)
#pragma warning(disable : 26446)

//TODO : No const correctness
namespace {
	constexpr bool IsValidInputChar(int key) {
		return key >= 32 && key <= 125;
	}

	bool CanAddCharacter(const std::string& str, size_t maxLength) {
		return str.size() < maxLength;
	}

	bool IsAtMaxCapacity(const std::string& str, size_t maxLength) {
		return str.size() >= maxLength;
	}
}

Game::Game() : resources(), background()
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

Projectile::Projectile(Vector2 startPos, EntityType projType, int speed) : position(startPos), type(projType), speed(speed) {
	lineStart = { position.x, position.y - GameConstants::Projectile::LENGTH };
	lineEnd = { position.x, position.y + GameConstants::Projectile::LENGTH };
}
Wall::Wall(Vector2 pos) : position(pos) {
}

Alien::Alien(float x, float y) : position({ x, y }) {
}

void Game::Start()
{
	player = Player(static_cast<float>(GetScreenWidth()));
	score = 0;
	SpawnAliens();
	SpawnWalls();
	gameState = State::GAMEPLAY;

}

//TODO : Add error handling
void Game::End()
{
	//SAVE SCORE AND UPDATE SCOREBOARD
	projectiles.clear();
	walls.clear();
	aliens.clear();
	newHighScore = CheckNewHighScore();
	gameState = State::ENDSCREEN;
}

void Game::Continue()
{
	SaveLeaderboard();
	gameState = State::STARTSCREEN;
}

void Game::Update()
{
	switch (gameState)
	{
	case State::STARTSCREEN:

		UpdateStartScreen();
		break;
	case State::GAMEPLAY:
		UpdateGameplay();

		break;
	case State::ENDSCREEN:
		UpdateEndScreen();
		break;
	default:
		break;
	}
}

void Game::UpdateStartScreen()
{
	if (IsKeyReleased(KEY_SPACE))Start();
}

void Game::UpdateGameplay()
{
	if (IsKeyReleased(KEY_Q))
	{
		End();
		return;
	}
	CheckGameCollisions();
	UpdateEntities();
	AlienShooting();
	HandlePlayerInput();
	UpdateBackground();
	LoseConditions();
	RemoveInactiveEntities();
	SpawnNewWave();
}

void Game::UpdateEndScreen()
{
	if (IsKeyReleased(KEY_ENTER) && !newHighScore)
	{
		Continue();
	}
	EntryName();
}

void Game::EntryName() {
	if (newHighScore)
	{
		using namespace GameConstants::UI::EndScreen::NameEntry;

		const Rectangle textBox = { TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT };
		const Vector2 mousePos = GetMousePosition();
		mouseOnText = CheckCollisionPointRec(mousePos, textBox);

		if (mouseOnText)
		{
			SetMouseCursor(MOUSE_CURSOR_IBEAM);

			int key = GetCharPressed();
			while (key > 0)
			{
				if (IsValidInputChar(key) && CanAddCharacter(name, GameConstants::UI::MAX_NAME_LENGTH))
				{
					name.push_back(static_cast<char>(key));
				}
				key = GetCharPressed();
			}

			if (IsKeyPressed(KEY_BACKSPACE) && !name.empty())
			{
				name.pop_back();
			}
		}
		else
		{
			SetMouseCursor(MOUSE_CURSOR_DEFAULT);
		}

		framesCounter = mouseOnText ? framesCounter + 1 : 0;

		if (!name.empty() && IsKeyReleased(KEY_ENTER))
		{
			InsertNewHighScore(name);
			newHighScore = false;
			name.clear();
		}
	}
}

void Game::HandlePlayerInput()
{
	if (IsKeyPressed(KEY_SPACE))
	{
		const Vector2 startPos = {
			player.GetXPos(),
			static_cast<float>(GetScreenHeight()) - GameConstants::Player::Shooting::SPAWN_Y_OFFSET
		};
		projectiles.emplace_back(startPos, EntityType::PLAYER_PROJECTILE);

	}
	player.Update();
}

void Game::UpdateEntities() {
	for (int i = 0; i < projectiles.size(); i++)
	{
		projectiles[i].Update();
	}
	for (int i = 0; i < walls.size(); i++)
	{
		walls[i].Update();
	}
}

void Game::UpdateBackground() {
	playerPos = { player.GetXPos(), GameConstants::Player::BASE_HEIGHT };
	cornerPos = { 0, GameConstants::Player::BASE_HEIGHT };
	offset = CollisionSystem::CalculateLineLength(playerPos, cornerPos) * -1;
	background.Update(offset / GameConstants::Background::PARALLAX_SPEED);
}

void Game::LoseConditions()
{
	if (player.lives < 1)
	{
		End();
	}
	for (int i = 0; i < aliens.size(); i++)
	{
		aliens[i].Update();

		if (aliens[i].GetPosition().y > GetScreenHeight() - GameConstants::Player::BASE_HEIGHT)
		{
			End();
		}
	}
}

void Game::RemoveInactiveEntities() {
	projectiles.erase(
		std::remove_if(
			projectiles.begin(),
			projectiles.end(),
			[](const Projectile& proj) { return !proj.IsActive(); }
		),
		projectiles.end()
	);

	aliens.erase(
		std::remove_if(
			aliens.begin(),
			aliens.end(),
			[](const Alien& alien) { return !alien.IsActive(); }
		),
		aliens.end()
	);
	walls.erase(
		std::remove_if(
			walls.begin(),
			walls.end(),
			[](const Wall& wall) { return !wall.IsActive(); }
		),
		walls.end()
	);

}

void Game::AlienShooting() {
	shootTimer += 1;
	if (shootTimer > GameConstants::Alien::Shooting::INTERVAL_FRAMES) //once per second
	{
		int randomAlienIndex = 0;

		if (aliens.size() > 1)
		{
			randomAlienIndex = rand() % aliens.size();
		}
		Vector2 shootPos = aliens[randomAlienIndex].GetPosition();
		shootPos.y += GameConstants::Alien::Shooting::Y_OFFSET;
		projectiles.emplace_back(shootPos, EntityType::ALIEN_PROJECTILE, GameConstants::Alien::Shooting::PROJECTILE_SPEED);
		shootTimer = 0;
	}
}

void Game::SpawnWalls()
{
	walls.clear();
	walls.reserve(wallCount);
	for (size_t i = 0; i < wallCount; i++)
	{
		const float x = static_cast<float>((GetScreenWidth() / (wallCount + 1)) * (i + 1));
		const float y = static_cast<float>(GetScreenHeight() - GameConstants::Wall::Y_OFFSET);
		walls.emplace_back(Vector2{ x, y });
	}
}

void Game::SpawnAliens()
{
	aliens.clear();
	aliens.reserve(GameConstants::Formation::WIDTH * GameConstants::Formation::HEIGHT);

	for (int row = 0; row < GameConstants::Formation::HEIGHT; ++row) {
		for (int col = 0; col < GameConstants::Formation::WIDTH; ++col) {
			const float x = static_cast<float>(
				GameConstants::Formation::START_X +
				GameConstants::Formation::OFFSET_X +
				(col * GameConstants::Formation::SPACING)
				);
			const float y = static_cast<float>(
				GameConstants::Formation::START_Y +
				(row * GameConstants::Formation::SPACING)
				);

			aliens.emplace_back(x, y);
		}
	}

}

void Game::SpawnNewWave() {
	if (aliens.empty()) {
		SpawnAliens();
	}

}

void Game::CheckGameCollisions()
{
	CheckPlayerProjectileCollisions();
	CheckAlienProjectileCollisions();
	CheckWallCollisions();
}

void Game::CheckPlayerProjectileCollisions()
{
	for (auto& projectile : projectiles)
	{
		if (projectile.GetType() != EntityType::PLAYER_PROJECTILE || !projectile.IsActive()) continue;
		for (auto& alien : aliens) {
			if (!alien.IsActive()) continue;

			if (CollisionSystem::CheckCollision(alien.GetPosition(), GameConstants::Alien::RADIUS, projectile.GetLineStart(), projectile.GetLineEnd()))
			{
				// Handle collision
				projectile.SetInactive();
				alien.SetInactive();
				score += GameConstants::Scoring::POINTS_PER_ALIEN;
				break;
			}
		}

	}
}

void Game::CheckAlienProjectileCollisions()
{
	const Vector2 playerPosition = { player.GetXPos(), static_cast<float>(GetScreenHeight()) - GameConstants::Player::BASE_HEIGHT };

	for (auto& projectile : projectiles)
	{
		if (projectile.GetType() != EntityType::ALIEN_PROJECTILE || !projectile.IsActive()) continue;
		if (CollisionSystem::CheckCollision(playerPosition, GameConstants::Player::BASE_HEIGHT, projectile.GetLineStart(), projectile.GetLineEnd()))
		{
			projectile.SetInactive();
			player.lives -= 1;
		}
	}
}

void Game::CheckWallCollisions() {
	for (auto& projectile : projectiles)
	{
		if (!projectile.IsActive()) continue;
		for (auto& wall : walls)
		{
			if (!wall.IsActive()) continue;
			if (CollisionSystem::CheckCollision(wall.GetPosition(), GameConstants::Wall::RADIUS, projectile.GetLineStart(), projectile.GetLineEnd()))
			{
				projectile.SetInactive();
				wall.takeDamage();
				break;
			}
		}
	}
}

void Game::Render() const
{

	switch (gameState)
	{
	case State::STARTSCREEN:
		RenderStart();
		break;
	case State::GAMEPLAY:
		RenderGameplay();
		break;
	case State::ENDSCREEN:
		RenderEnd();
		break;
	default:

		break;
	}
}

void Game::RenderStart() const {
	using namespace GameConstants::UI;

	DrawText("SPACE INVADERS", StartScreen::TITLE_X, StartScreen::TITLE_Y, StartScreen::TITLE_SIZE, YELLOW);

	DrawText("PRESS SPACE TO BEGIN", StartScreen::PROMPT_X, StartScreen::PROMPT_Y, StartScreen::PROMPT_SIZE, YELLOW);


}

void Game::RenderGameplay() const {

	using namespace GameConstants::UI::HUD;
	background.Render();

	DrawText(TextFormat("Score: %i", score), SCORE_X, SCORE_Y, TEXT_SIZE, YELLOW);
	DrawText(TextFormat("Lives: %i", player.lives), LIVES_X, LIVES_Y, TEXT_SIZE, YELLOW);


	player.Render(resources);


	for (const auto& projectile : projectiles)
	{
		projectile.Render(resources);
	}

	for (const auto& wall : walls)
	{
		wall.Render(resources);
	}

	for (const auto& alien : aliens)
	{
		alien.Render(resources);
	}


}

void Game::RenderEnd() const {
	using namespace GameConstants::UI::EndScreen;
	if (newHighScore)
	{
		RenderEntryName();
	}
	else {
		DrawText("PRESS ENTER TO CONTINUE", HIGHSCORE_X, CONTINUE_Y, CONTINUE_SIZE, YELLOW);
		RenderLeaderboard();
	}
}

void Game::DrawTextbox() const {
	using namespace GameConstants::UI::EndScreen::NameEntry;

	DrawRectangle(TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT, LIGHTGRAY);
	const Color borderColor = mouseOnText ? RED : DARKGRAY;
	DrawRectangleLines(TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT, borderColor);
}

void Game::DrawNameText() const {
	using namespace GameConstants::UI::EndScreen::NameEntry;

	const int textX = TEXTBOX_X + TEXT_X_OFFSET;
	const int textY = TEXTBOX_Y + TEXT_Y_OFFSET;
	DrawText(name.c_str(), textX, textY, TEXT_SIZE, MAROON);
}

void Game::DrawCharacterCount() const {
	using namespace GameConstants::UI::EndScreen::NameEntry;

	const size_t currentLength = name.size();
	const std::string countText = "INPUT CHARS: " + std::to_string(currentLength) +
		"/" + std::to_string(GameConstants::UI::MAX_NAME_LENGTH);
	DrawText(countText.c_str(), TEXTBOX_X, COUNT_Y, PROMPT_SIZE, YELLOW);
}

void Game::DrawCursorAndHints() const {
	using namespace GameConstants::UI::EndScreen::NameEntry;

	if (!mouseOnText) return;

	const bool atMaxCapacity = IsAtMaxCapacity(name, GameConstants::UI::MAX_NAME_LENGTH);
	const bool shouldShowCursor = (framesCounter / GameConstants::UI::TEXT_BLINK_INTERVAL) % 2 == 0;

	if (!atMaxCapacity && shouldShowCursor)
	{
		const int textX = TEXTBOX_X + TEXT_X_OFFSET;
		const int textY = TEXTBOX_Y + TEXT_Y_OFFSET;
		const int cursorX = textX + MeasureText(name.c_str(), TEXT_SIZE);
		DrawText("_", cursorX, textY, TEXT_SIZE, MAROON);
	}
}

void Game::DrawContinuePrompt() const {
	using namespace GameConstants::UI;
	using namespace GameConstants::UI::EndScreen::NameEntry;

	const bool canContinue = !name.empty() && !IsAtMaxCapacity(name, MAX_NAME_LENGTH);
	if (canContinue)
	{
		DrawText("PRESS ENTER TO CONTINUE", TEXTBOX_X, CONTINUE_PROMT_Y,
			EndScreen::CONTINUE_SIZE, YELLOW);
	}
}

void Game::RenderEntryName() const {
	using namespace GameConstants::UI;
	using namespace GameConstants::UI::EndScreen::NameEntry;

	DrawText("NEW HIGHSCORE!", EndScreen::HIGHSCORE_X, EndScreen::HIGHSCORE_Y,
		EndScreen::HIGHSCORE_SIZE, YELLOW);
	DrawText("PLACE MOUSE OVER INPUT BOX!", TEXTBOX_X, PROMPT_Y, PROMPT_SIZE, YELLOW);

	DrawTextbox();
	DrawNameText();
	DrawCharacterCount();
	DrawCursorAndHints();
	DrawContinuePrompt();
}

void Game::RenderLeaderboard() const {
	using namespace GameConstants::UI;
	DrawText("PRESS ENTER TO CONTINUE", EndScreen::HIGHSCORE_X, EndScreen::CONTINUE_Y, EndScreen::CONTINUE_SIZE, YELLOW);

	DrawText("LEADERBOARD", Leaderboard::TITLE_X, Leaderboard::TITLE_Y, HUD::TEXT_SIZE, YELLOW);

	for (int i = 0; i < Leaderboard.size(); i++)
	{
		const char* tempNameDisplay = Leaderboard[i].name.data();
		DrawText(tempNameDisplay, Leaderboard::NAME_X, Leaderboard::START_Y + (i * Leaderboard::ROW_HEIGHT), HUD::TEXT_SIZE, YELLOW);
		DrawText(TextFormat("%i", Leaderboard[i].score), Leaderboard::SCORE_X, Leaderboard::START_Y + (i * Leaderboard::ROW_HEIGHT), HUD::TEXT_SIZE, YELLOW);
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

void Player::Render(const Resources& resources) const
{
	using namespace GameConstants::Player;
	float window_height = static_cast<float>(GetScreenHeight());

	DrawTexturePro(resources.GetShipTexture(activeTexture),
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
	position.y -= static_cast<float>(speed);

	lineStart = { position.x, position.y - LENGTH };
	lineEnd = { position.x, position.y + LENGTH };

	if (position.y < 0.f || position.y > OUT_OF_BOUNDS)
	{
		active = false;
	}
}

void Projectile::Render(const Resources& resources) const
{
	using namespace GameConstants::Projectile::Rendering;
	DrawTexturePro(resources.GetProjectileTexture(),
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

void Wall::Render(const Resources& resources) const
{
	using namespace GameConstants::Wall::Rendering;
	DrawTexturePro(resources.GetWallTexture(),
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

	if (health < 1)
	{
		active = false;
	}


}

void Wall::takeDamage() noexcept
{
	health--;
}

void Alien::Update()
{
	using namespace GameConstants::Alien;
	const float screenWidth = static_cast<float>(GetScreenWidth());
	if (moveRight)
	{
		position.x += SPEED;

		if (position.x >= screenWidth)
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

void Alien::Render(const Resources& resources) const
{
	using namespace GameConstants::Alien::Rendering;

	DrawTexturePro(resources.GetAlienTexture(),
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
	for (auto& star : stars)
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

