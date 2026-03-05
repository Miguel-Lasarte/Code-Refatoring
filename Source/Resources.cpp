#include "Resources.h"

Resources::Resources()
	: shipTextures{ ResourceHandle<Texture2D>{"./Assets/Ship1.png"},
					  ResourceHandle<Texture2D>{"./Assets/Ship2.png"},
					  ResourceHandle<Texture2D>{"./Assets/Ship3.png"} }
	, alienTexture{ "./Assets/Alien.png" }
	, wallTexture{ "./Assets/Barrier.png" }
	, projectileTexture{ "./Assets/Laser.png" }
{
}

const Texture2D& Resources::GetShipTexture(std::size_t index) const 
{
	if (index >= shipTextures.size())
		throw std::out_of_range("Ship texture index out of bounds: " + std::to_string(index));
	return shipTextures[index].Get();
}