#pragma once 
#include <iostream>
#include <string>
#include <fstream>


//TODO : Delete this struct if not used
struct Entity
{
	float x, y;

	void LoadLevelFromAFile(const std::string& filename);
};