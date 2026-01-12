#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>
#include "Constants.h"
#include "CollisionSystem.h"

//TODO : No private members in structs
//TODO : Missing const correctness
//TODO : A lot of magic numbers
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
	void Render(Texture2D resources);
	[[nodiscard]] float GetXPos() const noexcept { return xPos; }
	[[nodiscard]] Vector2 GetPosition() const noexcept {
		return { xPos,static_cast<float>(GetScreenHeight()) - GameConstants::Player::BASE_HEIGHT};
	}
	[[nodiscard]] size_t GetActiveTextureIndex() const noexcept { return activeTexture; }
};

struct Projectile
{
public: 
	// INITIALIZE PROJECTILE WHILE DEFINING IF ITS PLAYER OR ENEMY 
	Vector2 position = {0,0};
	bool active = true; 
	float speed = GameConstants::Projectile::SPEED;
	EntityType type = {};

	// LINE WILL UPDATE WITH POSITION FOR CALCULATIONS
	Vector2 lineStart = { 0, 0 };
	Vector2 lineEnd = { 0, 0 };

	void Update();

	void Render(Texture2D texture);
};

struct Wall 
{
public: 
	Vector2 position; 
	//TODO Unused Rectangle variable
	Rectangle rec; 
	bool active; 
	Color color; 
	int health = GameConstants::Wall::INITIAL_HEALTH;


	void Render(Texture2D texture); 
	void Update(); 
};

struct Alien
{
public:
	
	Color color = WHITE; 
	Vector2 position = {0, 0};
	//TODO : Unused x and y variables
	int x = 0; 
	int y = 0; 
	bool active = true;  
	bool moveRight = true; 
	
	EntityType type = EntityType::ENEMY; 

		 
	void Update(); 
	void Render(Texture2D texture); 
};


struct Star
{
	Vector2 initPosition = { 0, 0 };
	Vector2 position = { 0, 0 };
	Color color = GRAY;
	float size = 0;
	void Update(float starOffset);
	void Render();
};

struct Background
{
	

	std::vector<Star> Stars;

	void Initialize(int starAmount);
	void Update(float offset);
	void Render();

};

struct Game
{private:
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