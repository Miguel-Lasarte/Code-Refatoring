#include "game.h"
#include <fstream>
#include <algorithm>
#include <random>


namespace {
	constexpr bool IsValidInputChar(int key) {
		return key >= 32 && key <= 125;
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
	}
}


void Game::TransitionToGameplay()
{
	score = 0;
	shootTimer = 0;
	playerProjectiles.clear();
	alienProjectiles.clear();
	player = Player(static_cast<float>(GetScreenWidth()));
	walls.clear();
	aliens.clear();
	SpawnAliens();
	SpawnWalls();
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

void Game::UpdateGameplay()
{
	if (IsKeyReleased(KEY_Q))
	{
		TransitionToEnd();
		return;
	}
	ProcessGameLogic();
	HandlePlayerInput();
}


void Game::UpdateEnd()
{
	if (IsKeyReleased(KEY_ENTER) && !newHighScore)
		TransitionToStart();

	EntryName();
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
			player.GetPosition().x,
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
	for (auto& alien : aliens) {
		alien.Update();
	}
}

void Game::LoseConditions()
{
	if (player.GetLives() < 1)
	{
		TransitionToEnd();
		return;
	}
	const float screenBottom = static_cast<float>(GetScreenHeight() - GameConstants::Player::BASE_HEIGHT);
	for (const auto& alien : aliens)
	{
		if (alien.GetPosition().y > screenBottom)
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
}

void Game::AlienShooting() {
	if (aliens.empty()) return;

	shootTimer += 1;
	if (shootTimer > GameConstants::Alien::Shooting::INTERVAL_FRAMES) {
		thread_local static std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<size_t> dist(0, aliens.size() - 1);
		Vector2 shootPos = aliens[dist(rng)].GetPosition();
		shootPos.y += GameConstants::Alien::Shooting::Y_OFFSET;
		alienProjectiles.emplace_back(shootPos, GameConstants::Alien::Shooting::PROJECTILE_SPEED);
		shootTimer = 0;
	}
}

void Game::SpawnWalls()
{
	walls.reserve(wallCount);
	for (int i = 0; i < wallCount; i++)
	{
		const float x = static_cast<float>((GetScreenWidth() / (wallCount + 1)) * (i + 1));
		const float y = static_cast<float>(GetScreenHeight() - GameConstants::Wall::Y_OFFSET);
		walls.emplace_back(Vector2{ x, y });
	}
}

void Game::SpawnAliens()
{
	using namespace GameConstants::Formation;
	aliens.reserve(static_cast<std::size_t>(WIDTH) * static_cast<std::size_t>(HEIGHT));

	for (int row = 0; row < HEIGHT; ++row) {
		for (int col = 0; col < WIDTH; ++col) {
			const float x = static_cast<float>(START_X + OFFSET_X + (col * SPACING));
			const float y = static_cast<float>(START_Y +(row * SPACING));
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
	CheckPlayerProjectileVsAlien();
	CheckAlienProjectileVsPlayer();
	CheckProjectileVsWall(playerProjectiles);
	CheckProjectileVsWall(alienProjectiles);
}

void Game::CheckPlayerProjectileVsAlien()
{
	for (auto& proj : playerProjectiles)
	{
		if (!proj.IsActive()) continue;
		for (auto& alien : aliens)
		{
			if (!alien.IsActive()) continue;
			if (CollisionSystem::Check(proj.GetBounds(), alien.GetBounds()))
			{
				proj.SetInactive();
				alien.SetInactive();
				score += GameConstants::Scoring::POINTS_PER_ALIEN;
				break;
			}
		}
	}
}

void Game::CheckAlienProjectileVsPlayer()
{
	const Rectangle playerBounds = player.GetBounds();
	for (auto& proj : alienProjectiles)
	{
		if (!proj.IsActive()) continue;
		if (CollisionSystem::Check(proj.GetBounds(), playerBounds))
		{
			proj.SetInactive();
			player.TakeDamage();
			break;
		}
	}
}

void Game::CheckProjectileVsWall(std::vector<Projectile>& projectiles) {

	for (auto& proj : projectiles) {
		if (!proj.IsActive()) continue;
		for (auto& wall : walls) {
			if (!wall.IsActive()) continue;
			if (CollisionSystem::Check(proj.GetBounds(), wall.GetBounds()))
			{
				wall.TakeDamage();
				proj.SetInactive();
				break;
			}
		}
	}
}


void Game::EntryName() {

	if (!newHighScore) return;

	using namespace GameConstants::UI::EndScreen::NameEntry;

	const Rectangle textBox = { TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT };
	const Vector2 mousePos = GetMousePosition();
	mouseOnText = CheckCollisionPointRec(mousePos, textBox);

	SetMouseCursor(mouseOnText ? MOUSE_CURSOR_IBEAM : MOUSE_CURSOR_DEFAULT);
	if (mouseOnText)
	{
		HandleTextInput();
		++framesCounter;
	}
	else
	{
		framesCounter = 0;
	}

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
		if (IsValidInputChar(key) && name.size() < static_cast<std::size_t>(GameConstants::UI::MAX_NAME_LENGTH))
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

bool Game::CheckNewHighScore() const noexcept
{
	
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
	int playerScore = 0;

	while (file >> playerName >> playerScore)
	{
		leaderboard.push_back({ playerName, playerScore });
	}

	const auto target = GameConstants::UI::LEADERBOARD_SIZE;
	while (leaderboard.size() < target)
	{
		leaderboard.push_back({ "Player", 0 });
	}
}

void Game::SaveLeaderboard()
{
	
	std::ofstream file(GameConstants::Files::LEADERBOARD_PATH.data(), std::ios::trunc);
	if (!file.is_open())
	{
		TraceLog(LOG_ERROR, "Failed to open leaderboard file for writing");
		return;
	}

	for (const auto& entry : leaderboard)
		file << entry.name << " " << entry.score << "\n";
}

void Game::RenderStart() const {
	using namespace GameConstants::UI::StartScreen;

	DrawText("SPACE INVADERS", TITLE_X, TITLE_Y, TITLE_SIZE, YELLOW);

	DrawText("PRESS SPACE TO BEGIN", PROMPT_X, PROMPT_Y, PROMPT_SIZE, YELLOW);


}

void Game::RenderGameplay() const {

	using namespace GameConstants::UI::HUD;

	background.Render(player.GetPosition().x);

	DrawText(TextFormat("Score: %i", score), SCORE_X, SCORE_Y, TEXT_SIZE, YELLOW);
	DrawText(TextFormat("Lives: %i", player.GetLives()), LIVES_X, LIVES_Y, TEXT_SIZE, YELLOW);

	player.Render(resources);

	for (const auto& proj : playerProjectiles) proj.Render(resources);
	for (const auto& proj : alienProjectiles)  proj.Render(resources);
	for (const auto& wall : walls)             wall.Render(resources);
	for (const auto& alien : aliens)           alien.Render(resources);
}

void Game::RenderEnd() const {
	if (newHighScore)
	{
		RenderNameEntry();
	}
	else {
		RenderLeaderboard();
	}
}


void Game::RenderNameEntry() const
{
	using namespace GameConstants::UI::EndScreen;
	using namespace NameEntry;

	DrawText("NEW HIGHSCORE!", HIGHSCORE_X, HIGHSCORE_Y, HIGHSCORE_SIZE, YELLOW);
	DrawText("PLACE MOUSE OVER INPUT BOX!", TEXTBOX_X, PROMPT_Y, PROMPT_SIZE, YELLOW);

	// Textbox
	DrawRectangle(TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT, LIGHTGRAY);
	DrawRectangleLines(TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
		mouseOnText ? RED : DARKGRAY);

	DrawText(name.c_str(),
		TEXTBOX_X + TEXT_X_OFFSET, TEXTBOX_Y + TEXT_Y_OFFSET,
		TEXT_SIZE, MAROON);

	const std::string countText =
		"INPUT CHARS: " + std::to_string(name.size()) +
		"/" + std::to_string(GameConstants::UI::MAX_NAME_LENGTH);
	DrawText(countText.c_str(), TEXTBOX_X, COUNT_Y, PROMPT_SIZE, YELLOW);

	const bool atMax = name.size() >= static_cast<std::size_t>(GameConstants::UI::MAX_NAME_LENGTH);
	const bool showCursor = (framesCounter / GameConstants::UI::TEXT_BLINK_INTERVAL) % 2 == 0;
	if (mouseOnText && !atMax && showCursor)
	{
		const int cursorX = TEXTBOX_X + TEXT_X_OFFSET + MeasureText(name.c_str(), TEXT_SIZE);
		DrawText("_", cursorX, TEXTBOX_Y + TEXT_Y_OFFSET, TEXT_SIZE, MAROON);
	}

	if (!name.empty())
		DrawText("PRESS ENTER TO CONTINUE", TEXTBOX_X, CONTINUE_PROMT_Y, CONTINUE_SIZE, YELLOW);
}

void Game::RenderLeaderboard() const
{
	using namespace GameConstants::UI;

	DrawText("PRESS ENTER TO CONTINUE",
		EndScreen::HIGHSCORE_X, EndScreen::CONTINUE_Y, EndScreen::CONTINUE_SIZE, YELLOW);
	DrawText("LEADERBOARD",
		Leaderboard::TITLE_X, Leaderboard::TITLE_Y, HUD::TEXT_SIZE, YELLOW);

	for (std::size_t i = 0; i < leaderboard.size(); ++i)
	{
		const int y = Leaderboard::START_Y + static_cast<int>(i) * Leaderboard::ROW_HEIGHT;
		DrawText(leaderboard[i].name.c_str(), Leaderboard::NAME_X, y, HUD::TEXT_SIZE, YELLOW);
		DrawText(TextFormat("%i", leaderboard[i].score), Leaderboard::SCORE_X, y, HUD::TEXT_SIZE, YELLOW);
	}
}
