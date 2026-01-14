#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>
#include "Constants.h"
#include "CollisionSystem.h"
#include <optional>


enum struct State
{
	STARTSCREEN,
	GAMEPLAY,
	ENDSCREEN
};

enum struct EntityType
{
	PLAYER,
	ALIEN,
	PLAYER_PROJECTILE,
	ALIEN_PROJECTILE
};

struct PlayerData
{
	std::string name;
	int score;
};

class Player {
private:
	float xPos = 0.f;
	float timer = 0.f;
	size_t activeTexture = 0;
	int direction = 0;
public:
	int  lives = GameConstants::Player::INITIAL_LIVES;

	explicit Player(float screenWidth);
	void Update();
	void Render(const Resources& resources)const;
	[[nodiscard]] float GetXPos() const noexcept { return xPos; }
	[[nodiscard]] Vector2 GetPosition() const noexcept {
		return { xPos,static_cast<float>(GetScreenHeight()) - GameConstants::Player::BASE_HEIGHT };
	}
	[[nodiscard]] size_t GetActiveTextureIndex() const noexcept { return activeTexture; }
};

class Projectile
{
private:
	Vector2 position = { 0.f,0.f };
	bool active = true;
	int speed;
	EntityType type = {};
	Vector2 lineStart = { 0, 0 };
	Vector2 lineEnd = { 0, 0 };

public:

	Projectile(Vector2 startPos, EntityType projType, int speed = GameConstants::Projectile::SPEED);
	void Update();
	void Render(const Resources& resources) const;

	[[nodiscard]] bool IsActive() const noexcept { return active; }
	[[nodiscard]] EntityType GetType() const noexcept { return type; }
	[[nodiscard]] Vector2 GetLineStart() const noexcept { return lineStart; }
	[[nodiscard]] Vector2 GetLineEnd() const noexcept { return lineEnd; }

	void SetInactive() noexcept { active = false; }
};

class Wall
{
private:
	Vector2 position = { 0.f,0.f };
	int health = GameConstants::Wall::INITIAL_HEALTH;
	bool active = true;

public:

	explicit Wall(Vector2 pos);

	void Update();
	void Render(const Resources& resources) const;

	[[nodiscard]] Vector2 GetPosition() const noexcept { return position; }
	[[nodiscard]] bool IsActive() const noexcept { return active; }

	void takeDamage() noexcept;
};

class Alien
{
private:
	Vector2 position = { 0.f, 0.f };
	bool active = true;
	bool moveRight = true;
public:

	Alien(float x, float y);

	void Update();
	void Render(const Resources& resources) const;

	[[nodiscard]] Vector2 GetPosition() const noexcept { return position; }
	[[nodiscard]] bool IsActive() const noexcept { return active; }
	void SetInactive() noexcept { active = false; }
};

class Star
{
private:
	Vector2 initPosition = { 0.f, 0.f };
	Vector2 position = { 0.f, 0.f };
	Color color = GRAY;
	float size = 0.f;
public:
	Star(Vector2 pos, float size);
	void Update(float fffset);
	void Render() const;
};

class Background
{
private:
	std::vector<Star> stars;
public:
	Background();
	void Update(float offset);
	void Render() const;

};

class Game
{
private:
	Resources resources;

	void Start();
	void End();

	void Continue();

	void UpdateStartScreen();
	void UpdateGameplay();
	void UpdateEndScreen();
	void ProcessGameLogic();
	void HandlePlayerInput();
	void UpdateEntities();
	void UpdateBackground();
	void LoseConditions();
	void RemoveInactiveEntities();
	void AlienShooting();
	void SpawnWalls();
	void SpawnNewWave();
	void SpawnPlayerProjectile();
	void EntryName();

	void CheckGameCollisions();
	void CheckPlayerProjectileCollisions();
	void CheckAlienProjectileCollisions();
	void CheckWallCollisions();

	void RenderStart() const;
	void RenderGameplay() const;
	void RenderEnd() const;
	void DrawTextbox() const;
	void DrawNameText() const;
	void DrawCharacterCount() const;
	void DrawCursor() const;
	void DrawContinuePrompt() const;
	void RenderEntryName() const;
	void RenderLeaderboard() const;
	void DrawNameInputBox() const;

	State gameState = {};

	int score;

	int wallCount = 5;

	float shootTimer = 0;

	bool newHighScore = false;

	void SpawnAliens();

	bool CheckNewHighScore() const noexcept;

	void InsertNewHighScore(const std::string& playerName);

	void LoadLeaderboard();
	void SaveLeaderboard();

	std::optional<Player> player;

	std::vector<Projectile> projectiles;

	std::vector<Wall> walls;

	std::vector<Alien> aliens;

	std::vector<PlayerData> leaderboard = { {"Player 1", 500}, {"Player 2", 400}, {"Player 3", 300}, {"Player 4", 200}, {"Player 5", 100} };

	Background background;

	std::string name = "";
	bool mouseOnText = false;

	int framesCounter = 0;

public:
	Game();
	~Game() = default;

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) noexcept = default;
	Game& operator=(Game&&) noexcept = default;

	void Update();
	void Render() const;


};