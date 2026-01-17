#pragma once
#include "raylib.h"
#include <vector>
#include "Constants.h"
#include "ResourceHandle.h"
#include <stdexcept>
#include <optional>


class Resources
{
	std::vector<ResourceHandle<Texture2D>> shipTextures;
	ResourceHandle<Texture2D> alienTexture;
	ResourceHandle<Texture2D> wallTexture;
	ResourceHandle<Texture2D> projectileTexture;
	
public:
	Resources();
	Resources(const Resources&) = delete;
	~Resources() = default;

	[[nodiscard]] const Texture2D& GetAlienTexture() const noexcept {
		return alienTexture.Get();
	}
	[[nodiscard]] const Texture2D& GetWallTexture() const noexcept {
		return wallTexture.Get();
	}
	[[nodiscard]] const Texture2D& GetProjectileTexture() const noexcept {
		return projectileTexture.Get();
	}
    [[nodiscard]] const Texture2D& GetShipTexture(size_t index) const noexcept;

};