#include "game.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>


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

Game::Game()
	: resources()
	, background()
	, gameState(State::STARTSCREEN)
	, score(0)
	, player(static_cast<float>(GetScreenWidth()))
	, leaderboard{ {"Player", 500}, {"Player", 400},
				  {"Player", 300}, {"Player", 200}, {"Player", 100} }
{
	LoadLeaderboard();
}


void Game::InitializeNewGame()
{
	score = 0;
	shootTimer = 0;
	playerProjectiles.clear();
	alienProjectiles.clear();
	walls.clear();
	aliens.clear();
	SpawnAliens();
	SpawnWalls();
}

void Game::TransitionToGameplay()
{
	InitializeNewGame();
	gameState = State::GAMEPLAY;
}

void Game::TransitionToEnd()
{
	playerProjectiles.clear();
	alienProjectiles.clear();
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
	player.Update();
}

void Game::SpawnPlayerProjectile()
{
	const Vector2 startPos = {
			player.GetXPos(),
			static_cast<float>(GetScreenHeight()) - GameConstants::Player::Shooting::SPAWN_Y_OFFSET
	};
	playerProjectiles.emplace_back(startPos, GameConstants::Projectile::SPEED);
}

void Game::UpdateEntities() {
	for (auto& proj : playerProjectiles) {
		proj.Update();
	}
	for (auto& proj : alienProjectiles) {
		proj.Update();
	}
	for (auto& wall : walls) {
		wall.Update();
	}
}

void Game::UpdateBackground() {
	Vector2 playerPos = { player.GetXPos(), GameConstants::Player::BASE_HEIGHT };
	Vector2 cornerPos = { 0, GameConstants::Player::BASE_HEIGHT };
	float offset = CollisionSystem::CalculateLineLength(playerPos, cornerPos) * -1;
	background.Update(offset / GameConstants::Background::PARALLAX_SPEED);
}

void Game::LoseConditions()
{
	if (player.lives < 1)
	{
		TransitionToEnd();
		return;
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

	std::erase_if(playerProjectiles, [](const auto& p) { return !p.IsActive(); });
	std::erase_if(alienProjectiles, [](const auto& p) { return !p.IsActive(); });
	std::erase_if(aliens, [](const auto& a) { return !a.IsActive(); });
	std::erase_if(walls, [](const auto& w) { return !w.IsActive(); });
};

void Game::AlienShooting() {
	if (aliens.empty()) return;

	shootTimer += 1;
	if (shootTimer > GameConstants::Alien::Shooting::INTERVAL_FRAMES) {
		const int randomIndex = aliens.size() > 1 ? rand() % aliens.size() : 0;
		Vector2 shootPos = aliens[randomIndex].GetPosition();
		shootPos.y += GameConstants::Alien::Shooting::Y_OFFSET;
		alienProjectiles.emplace_back(shootPos, GameConstants::Alien::Shooting::PROJECTILE_SPEED);
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
	using namespace GameConstants::Formation;
	aliens.clear();
	aliens.reserve(static_cast<std::vector<Alien, std::allocator<Alien>>::size_type>(WIDTH) * HEIGHT);

	for (int row = 0; row < HEIGHT; ++row) {
		for (int col = 0; col < WIDTH; ++col) {
			const float x = static_cast<float>(
				START_X +
				OFFSET_X +
				(col * SPACING)
				);
			const float y = static_cast<float>(
				START_Y +
				(row * SPACING)
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
	for (auto& projectile : playerProjectiles) {
		if (!projectile.IsActive()) continue;

		for (auto& alien : aliens) {
			if (!alien.IsActive()) continue;

			if (CollisionSystem::CheckCirclePointCollision(
				alien.GetPosition(),
				GameConstants::Alien::RADIUS,
				projectile.GetPosition()
			)) {
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
	const Vector2 playerPosition = player.GetPosition();

	for (auto& proj : alienProjectiles) {
		if (!proj.IsActive()) continue;

		if (CollisionSystem::CheckCirclePointCollision(
			playerPosition,
			GameConstants::Player::RADIUS,
			proj.GetPosition()
		)) {
			proj.SetInactive();
			player.lives -= 1;
			break;
		}
	}
}

void Game::CheckWallCollisions() {
	for (auto& proj : playerProjectiles) {
		if (!proj.IsActive()) continue;

		for (auto& wall : walls) {
			if (!wall.IsActive()) continue;

			if (CollisionSystem::CheckCirclePointCollision(
				wall.GetPosition(),
				GameConstants::Wall::RADIUS,
				proj.GetPosition()
			)) {
				wall.TakeDamage();
				proj.SetInactive();
				break;
			}
		}
	}

	for (auto& proj : alienProjectiles) {
		if (!proj.IsActive()) continue;

		for (auto& wall : walls) {
			if (!wall.IsActive()) continue;

			if (CollisionSystem::CheckCirclePointCollision(
				wall.GetPosition(),
				GameConstants::Wall::RADIUS,
				proj.GetPosition()
			)) {
				wall.TakeDamage();
				proj.SetInactive();
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
	using namespace GameConstants::UI::StartScreen;

	DrawText("SPACE INVADERS", TITLE_X, TITLE_Y, TITLE_SIZE, YELLOW);

	DrawText("PRESS SPACE TO BEGIN", PROMPT_X, PROMPT_Y, PROMPT_SIZE, YELLOW);


}

void Game::RenderGameplay() const {

	using namespace GameConstants::UI::HUD;
	background.Render();

	DrawText(TextFormat("Score: %i", score), SCORE_X, SCORE_Y, TEXT_SIZE, YELLOW);
	DrawText(TextFormat("Lives: %i", player.lives), LIVES_X, LIVES_Y, TEXT_SIZE, YELLOW);


	player.Render(resources);

	for (const auto& proj : playerProjectiles) {
		proj.Render(resources);
	}
	for (const auto& proj : alienProjectiles) {
		proj.Render(resources);
	}
	for (const auto& wall : walls) {
		wall.Render(resources);
	}
	for (const auto& alien : aliens) {
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

	for (size_t i = 0; i < leaderboard.size(); i++) {
        const int y = Leaderboard::START_Y + (static_cast<int>(i) * Leaderboard::ROW_HEIGHT);
        DrawText(leaderboard[i].name.c_str(), Leaderboard::NAME_X, y, 
                 HUD::TEXT_SIZE, YELLOW);
        DrawText(TextFormat("%i", leaderboard[i].score), Leaderboard::SCORE_X, y, 
                 HUD::TEXT_SIZE, YELLOW);
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
	std::ifstream file(GameConstants::Files::LEADERBOARD_PATH.data());
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
		std::ofstream file(GameConstants::Files::LEADERBOARD_PATH.data(), std::ios::trunc);
		if (!file.is_open()) {
			TraceLog(LOG_ERROR, "Failed to open leaderboard file for writing");
			return;
		}

		for (const auto& entry : leaderboard)
		{
			file << entry.name << " " << entry.score << "\n";
		}

		file.close();

		if (!file.good()) {
			TraceLog(LOG_ERROR, "Error occurred while writing leaderboard");
		}
	}
	catch (const std::exception& e) {
		TraceLog(LOG_ERROR, "Error saving leaderboard: %s", e.what());
	}
}

