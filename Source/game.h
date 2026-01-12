#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>
#include "Constants.h"
#include "CollisionSystem.h"

//TODO : Inconsistent naming conventions

enum struct State
{
	STARTSCREEN,
	GAMEPLAY,
	ENDSCREEN
};

enum struct EntityType
{
	PLAYER,
	ENEMY,
	PLAYER_PROJECTILE,
	ENEMY_PROJECTILE
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
	Player() = default;
	explicit Player(float screenWidth);
	void Update();
	void Render (const Resources& resources)const;
	[[nodiscard]] float GetXPos() const noexcept { return xPos; }
	[[nodiscard]] Vector2 GetPosition() const noexcept {
		return { xPos,static_cast<float>(GetScreenHeight()) - GameConstants::Player::BASE_HEIGHT };
	}
	[[nodiscard]] size_t GetActiveTextureIndex() const noexcept { return activeTexture; }
};

class Projectile
{
private:
	Vector2 position = { 0.f,0.f};
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
	void Render (const Resources& resources) const;

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

struct Game
{
private:
	Resources resources;
public:
	// Gamestate
	State gameState = {};

	// Score
	int score;

	// for later, make a file where you can adjust the number of walls (config file) 
	int wallCount = 5;

	//Aliens shooting
	float shootTimer = 0;

	//Aliens stuff? (idk cause liv wrote this)
	Rectangle rec = { 0, 0 ,0 ,0 };

	bool newHighScore = false;

	Game();
	void Start();
	void End();

	void Continue();


	void Update();
	void Render();

	void SpawnAliens();

	bool CheckNewHighScore();

	void InsertNewHighScore(std::string name);

	void LoadLeaderboard();
	void SaveLeaderboard();

	Player player;

	std::vector<Projectile> Projectiles;

	std::vector<Wall> Walls;

	std::vector<Alien> Aliens;

	std::vector<PlayerData> Leaderboard = { {"Player 1", 500}, {"Player 2", 400}, {"Player 3", 300}, {"Player 4", 200}, {"Player 5", 100} };

	Background background;



	Vector2 playerPos;
	Vector2 alienPos;
	Vector2 cornerPos;
	float offset;



	//TEXTBOX ENTER
	char name[9 + 1] = "\0";      //One extra space required for null terminator char '\0'
	int letterCount = 0;
	bool mouseOnText = false;

	int framesCounter = 0;

};