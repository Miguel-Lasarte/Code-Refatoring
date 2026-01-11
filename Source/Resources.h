#pragma once
#include "raylib.h"
#include "vector"
#include "Constants.h"


//TODO : No RAII for resources

struct Resources 
{
	//TODO: Two step initialization
	void Load();
	//void Unload();

	//TODO : Move the public member variables
	std::vector<Texture2D> shipTextures;
	Texture2D alienTexture;
	Texture2D barrierTexture;
	Texture2D laserTexture;

};