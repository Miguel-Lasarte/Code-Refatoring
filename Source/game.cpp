#include "game.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#pragma warning (push)
#pragma warning(disable : 26446)

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

Game::Game() : resources(), background(), gameState(State::STARTSCREEN), score(0)
{
	try {
		LoadLeaderboard();
	}
	catch (const std::exception& e) {
		std::cerr << "Error loading leaderboard: " << e.what() << std::endl;
		leaderboard = {
			{"Player 1", 500},
			{"Player 2", 400},
			{"Player 3", 300},
			{"Player 4", 200},
			{"Player 5", 100}
		};
	}
}

void Game::InitializeEntities()
{
	player.emplace(static_cast<float>(GetScreenWidth()));
	SpawnAliens();
	SpawnWalls();
}

void Game::InitializeNewGame()
{
	score = 0;
	shootTimer = 0;
	projectiles.clear();
	walls.clear();
	aliens.clear();
	InitializeEntities();
}

void Game::TransitionToGameplay()
{
	InitializeNewGame();
	gameState = State::GAMEPLAY;
}

void Game::TransitionToEnd()
{
	projectiles.clear();
	walls.clear();
	aliens.clear();
	newHighScore = CheckNewHighScore();
	gameState = State::ENDSCREEN;
}

void Game::TransitionToStart()
{
	SaveLeaderboard();
	name.clear();
	gameState = State::STARTSCREEN;
}

void Game::UpdateStart()
{
	if (IsKeyReleased(KEY_SPACE))
		TransitionToGameplay();
}

void Game::UpdateEnd()
{
	if (IsKeyReleased(KEY_ENTER) && !newHighScore)
		TransitionToStart();

	EntryName();
}

void Game::UpdateGameplay()
{
	if (IsKeyReleased(KEY_Q))
	{
		TransitionToEnd();
		return;
	}
	ProcessGameLogic();
	HandlePlayerInput();
	UpdateBackground();
}
void Game::Update()
{
	switch (gameState)
	{
	case State::STARTSCREEN:

		UpdateStart();
		break;
	case State::GAMEPLAY:
		UpdateGameplay();

		break;
	case State::ENDSCREEN:
		UpdateEnd();
		break;
	default:
		break;
	}
}


void Game::ProcessGameLogic()
{
	CheckGameCollisions();
	UpdateEntities();
	AlienShooting();
	LoseConditions();
	RemoveInactiveEntities();
	SpawnNewWave();
}

void Game::EntryName() {

	if (!newHighScore) return;

	using namespace GameConstants::UI::EndScreen::NameEntry;

	const Rectangle textBox = { TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT };
	const Vector2 mousePos = GetMousePosition();
	mouseOnText = CheckCollisionPointRec(mousePos, textBox);

	if (mouseOnText)
	{
		SetMouseCursor(MOUSE_CURSOR_IBEAM);
		HandleTextInput();
		
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

void Game::HandleTextInput() {
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

void Game::HandlePlayerInput()
{
	if (IsKeyPressed(KEY_SPACE))
	{
		SpawnPlayerProjectile();
	}
	player->Update();
}

void Game::SpawnPlayerProjectile()
{
	const Vector2 startPos = {
			player->GetXPos(),
			static_cast<float>(GetScreenHeight()) - GameConstants::Player::Shooting::SPAWN_Y_OFFSET
	};
	projectiles.emplace_back(startPos, EntityType::PLAYER_PROJECTILE);
}

void Game::UpdateEntities() {
	std::for_each(projectiles.begin(), projectiles.end(), [](auto& proj) {
		proj.Update();
		});
	std::for_each(walls.begin(), walls.end(), [](auto& wall) {
		wall.Update();
		});
}

void Game::UpdateBackground() {
	Vector2 playerPos = { player->GetXPos(), GameConstants::Player::BASE_HEIGHT };
	Vector2 cornerPos = { 0, GameConstants::Player::BASE_HEIGHT };
	float offset = CollisionSystem::CalculateLineLength(playerPos, cornerPos) * -1;
	background.Update(offset / GameConstants::Background::PARALLAX_SPEED);
}

void Game::LoseConditions()
{
	if (player->lives < 1)
	{
		TransitionToEnd();
	}
	for (auto& alien : aliens)
	{
		alien.Update();

		if (alien.GetPosition().y > GetScreenHeight() - GameConstants::Player::BASE_HEIGHT)
		{
			TransitionToEnd();
			return;
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
	if (shootTimer > GameConstants::Alien::Shooting::INTERVAL_FRAMES)
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
	aliens.reserve(static_cast<std::vector<Alien, std::allocator<Alien>>::size_type>(GameConstants::Formation::WIDTH) * GameConstants::Formation::HEIGHT);

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
	std::for_each(projectiles.begin(), projectiles.end(), [this](auto& projectile) {
		if (projectile.GetType() != EntityType::PLAYER_PROJECTILE || !projectile.IsActive())
			return;

		auto alienHit = std::find_if(aliens.begin(), aliens.end(), [&](auto& alien) {
			return alien.IsActive() &&
				CollisionSystem::CheckCollision(
					alien.GetPosition(),
					GameConstants::Alien::RADIUS,
					projectile.GetLineStart(),
					projectile.GetLineEnd()
				);
			});
		if (alienHit != aliens.end()) {
			projectile.SetInactive();
			alienHit->SetInactive();
			score += GameConstants::Scoring::POINTS_PER_ALIEN;
		}
		});
}

void Game::CheckAlienProjectileCollisions()
{
	const Vector2 playerPosition = { player->GetXPos(), static_cast<float>(GetScreenHeight()) - GameConstants::Player::BASE_HEIGHT };

	auto projectileHit = std::find_if(projectiles.begin(), projectiles.end(), [&](auto& proj) {
		return proj.GetType() == EntityType::ALIEN_PROJECTILE &&
			proj.IsActive() &&
			CollisionSystem::CheckCollision(
				playerPosition,
				GameConstants::Player::RADIUS,
				proj.GetLineStart(),
				proj.GetLineEnd()
			);
		});
	if (projectileHit != projectiles.end())
	{
		projectileHit->SetInactive();
		player->lives -= 1;
	}
}

void Game::CheckWallCollisions() {
	std::for_each(projectiles.begin(), projectiles.end(), [&](Projectile& proj) {
		if (!proj.IsActive()) return;
		auto wallHit = std::find_if(walls.begin(), walls.end(), [&](Wall& wall) {
			return wall.IsActive() &&
				CollisionSystem::CheckCollision(
					wall.GetPosition(),
					GameConstants::Wall::RADIUS,
					proj.GetLineStart(),
					proj.GetLineEnd()
				);
			});
		if (wallHit != walls.end())
		{
			wallHit->TakeDamage();
			proj.SetInactive();
		}
		});
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
	DrawText(TextFormat("Lives: %i", player->lives), LIVES_X, LIVES_Y, TEXT_SIZE, YELLOW);


	player->Render(resources);


	std::for_each(projectiles.begin(), projectiles.end(), [&](const auto& proj) {
		proj.Render(resources);
		});
	std::for_each(walls.begin(), walls.end(), [&](const auto& wall) {
		wall.Render(resources);
		});
	std::for_each(aliens.begin(), aliens.end(), [&](const auto& alien) {
		alien.Render(resources);
		});
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

void Game::DrawCursor() const {
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

	DrawNameInputBox();
	DrawContinuePrompt();

}

void Game::DrawNameInputBox() const {
	DrawTextbox();
	DrawNameText();
	DrawCharacterCount();
	DrawCursor();
}

void Game::RenderLeaderboard() const {
	using namespace GameConstants::UI;
	DrawText("PRESS ENTER TO CONTINUE", EndScreen::HIGHSCORE_X, EndScreen::CONTINUE_Y, EndScreen::CONTINUE_SIZE, YELLOW);

	DrawText("LEADERBOARD", Leaderboard::TITLE_X, Leaderboard::TITLE_Y, HUD::TEXT_SIZE, YELLOW);

	for (int i = 0; i < leaderboard.size(); i++)
	{
		const char* tempNameDisplay = leaderboard[i].name.data();
		DrawText(tempNameDisplay, Leaderboard::NAME_X, Leaderboard::START_Y + (i * Leaderboard::ROW_HEIGHT), HUD::TEXT_SIZE, YELLOW);
		DrawText(TextFormat("%i", leaderboard[i].score), Leaderboard::SCORE_X, Leaderboard::START_Y + (i * Leaderboard::ROW_HEIGHT), HUD::TEXT_SIZE, YELLOW);
	}
}

bool Game::CheckNewHighScore() const noexcept
{
	if (leaderboard.empty()) return true;
	if (leaderboard.size() < GameConstants::UI::LEADERBOARD_SIZE) return true;
	return score > leaderboard.back().score;
}

void Game::InsertNewHighScore(const std::string& playerName)
{
	PlayerData newEntry = { playerName, score };
	auto it = std::find_if(
		leaderboard.begin(), leaderboard.end(),
		[&newEntry](const PlayerData& entry) { return newEntry.score > entry.score; });

	leaderboard.insert(it, newEntry);

	if (leaderboard.size() > GameConstants::UI::LEADERBOARD_SIZE)
	{
		leaderboard.resize(GameConstants::UI::LEADERBOARD_SIZE);
	}
}

void Game::LoadLeaderboard()
{
	std::ifstream file(GameConstants::Files::LEADERBOARD_PATH);
	if (!file.is_open()) return;

	leaderboard.clear();
	std::string playerName;
	int playerScore;

	while (file >> playerName >> playerScore)
	{
		leaderboard.push_back({ playerName, playerScore });
	}

	while (leaderboard.size() < GameConstants::UI::LEADERBOARD_SIZE)
	{
		leaderboard.push_back({ "Player", 0 });
	}
}

void Game::SaveLeaderboard()
{
	try {
		std::ofstream file(GameConstants::Files::LEADERBOARD_PATH, std::ios::trunc);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to save Leaderboard.");
		}
		for (const auto& entry : leaderboard)
		{
			file << entry.name << " " << entry.score << "\n";
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error saving leaderboard: " << e.what() << std::endl;
		TraceLog(LOG_ERROR, "Error saving leaderboard: %s", e.what());
	}

}

#pragma warning (pop)
