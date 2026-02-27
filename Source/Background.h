#pragma once
#include "raylib.h"
#include "Constants.h"
#include <vector>

class Background
{
	struct Star
	{
		Vector2 position;   
		float   size;
	};

	std::vector<Star> stars;

public:
	Background();

	void Render(float playerX) const;
};