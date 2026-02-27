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

const Texture2D& Resources::GetShipTexture(std::size_t index) const noexcept
{
	if (index >= shipTextures.size())
	{
		TraceLog(LOG_ERROR, "Ship texture index out of bounds: %zu", index);
		return shipTextures[0].Get();
	}
	return shipTextures[index].Get();
}