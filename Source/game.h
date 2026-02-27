#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>
#include "Constants.h"
#include "CollisionSystem.h"
#include "Player.h"
#include "Projectiles.h"
#include "Wall.h"
#include "Alien.h"
#include "Background.h"


struct PlayerData
{
	std::string name;
	int score;
};

class Game
{
private:
	enum class State
	{
		STARTSCREEN,
		GAMEPLAY,
		ENDSCREEN
	};

	Resources resources;
	Background background;
	State gameState = State::STARTSCREEN;
	int score = 0;
	int shootTimer = 0;
	bool newHighScore = false;

	Player player;
	std::vector<Projectile> playerProjectiles;
	std::vector<Projectile> alienProjectiles;
	std::vector<Wall> walls;
	std::vector<Alien> aliens;

	

	std::vector<PlayerData> leaderboard;



	std::string name = "";
	bool mouseOnText = false;
	int framesCounter = 0;
	static constexpr int wallCount = 5;

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
	void LoseConditions();
	void RemoveInactiveEntities();
	void AlienShooting();
	void SpawnNewWave();
	void SpawnPlayerProjectile();

	void CheckGameCollisions();
	void CheckPlayerProjectileVsAlien();
	void CheckAlienProjectileVsPlayer();
	void CheckProjectileVsWall(std::vector<Projectile>& projectiles);

	void EntryName();
	void HandleTextInput();


	bool CheckNewHighScore() const noexcept;
	void InsertNewHighScore(const std::string& playerName);
	void LoadLeaderboard();
	void SaveLeaderboard();

	void RenderStart() const;
	void RenderGameplay() const;
	void RenderEnd() const;
	void RenderNameEntry() const;
	void RenderLeaderboard() const;


	

	

public:
	Game();
	~Game() = default;

	void Update();
	void Render() const;
};