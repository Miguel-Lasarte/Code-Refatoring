#pragma once
#include "raylib.h"
#include <vector>
#include "Constants.h"
#include "ResourceHandle.h"
#include <stdexcept>


class Resources
{
	std::vector<ResourceHandle<Texture2D>> shipTextures;
	ResourceHandle<Texture2D> alienTexture;
	ResourceHandle<Texture2D> wallTexture;
	ResourceHandle<Texture2D> projectileTexture;

public:
	Resources();

	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;
	Resources(Resources&&) noexcept = default;
	Resources& operator=(Resources&&) noexcept = default;
	~Resources() = default;

	[[nodiscard]] Texture2D GetAlienTexture() const noexcept {
		return alienTexture.Get();
	}
	[[nodiscard]] Texture2D GetWallTexture() const noexcept {
		return wallTexture.Get();
	}
	[[nodiscard]] Texture2D GetProjectileTexture() const noexcept {
		return projectileTexture.Get();
	}
	[[nodiscard]] Texture2D GetShipTexture(size_t index);

};