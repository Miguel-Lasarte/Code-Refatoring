#pragma once
#include "raylib.h"
#include "Constants.h"
#include <vector>

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
