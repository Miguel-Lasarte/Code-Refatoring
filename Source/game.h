#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>
#include "Constants.h"
#include "CollisionSystem.h"
#include <optional>
#include "Player.h"
#include "Projectiles.h"
#include "Wall.h"
#include "Alien.h"
#include "Background.h"


enum struct State
{
	STARTSCREEN,
	GAMEPLAY,
	ENDSCREEN
};


struct PlayerData
{
	std::string name;
	int score;
};

class Game
{
private:
	Resources resources;

	State gameState = {};

	int score;

	int wallCount = 5;

	float shootTimer = 0;

	bool newHighScore = false;

	Player player;

	std::vector<Projectile> playerProjectiles;

	std::vector<Projectile> alienProjectiles;

	std::vector<Projectile> projectiles;

	std::vector<Wall> walls;

	std::vector<Alien> aliens;

	std::vector<PlayerData> leaderboard;

	Background background;

	std::string name = "";
	bool mouseOnText = false;

	int framesCounter = 0;
	
	void InitializeNewGame();
	void TransitionToGameplay();
	void TransitionToEnd();
	void TransitionToStart();
	void SpawnAliens();
	void SpawnWalls();

	void UpdateStart();
	void UpdateGameplay();
	void UpdateEnd();
	void ProcessGameLogic();
	void HandlePlayerInput();
	void UpdateEntities();
	void UpdateBackground();
	void LoseConditions();
	void RemoveInactiveEntities();
	void AlienShooting();
	void SpawnNewWave();
	void SpawnPlayerProjectile();

	void EntryName();
	void HandleTextInput();

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

	bool CheckNewHighScore() const noexcept;

	void InsertNewHighScore(const std::string& playerName);
	void LoadLeaderboard();
	void SaveLeaderboard();

	

public:
	Game();
	~Game() = default;

	void Update();
	void Render() const;
};