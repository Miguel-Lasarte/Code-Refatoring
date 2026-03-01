#pragma once

#include "raylib.h"
#include <string_view>
#include <stdexcept>
#include <string>

template<typename T>
struct ResourceTraits;

template<>
struct ResourceTraits<Texture2D>
{
	static Texture2D Load(std::string_view path)
	{
		Texture2D tex = LoadTexture(path.data());
		if (tex.id == 0)
			throw std::runtime_error("Failed to load texture: " + std::string(path));
		return tex;
	}
	static void Unload(Texture2D& res) noexcept { UnloadTexture(res); }
};


template<typename T>
class ResourceHandle
{
	T resource;

public:
	explicit ResourceHandle(std::string_view path)
		: resource(ResourceTraits<T>::Load(path))
	{
	}

	~ResourceHandle() noexcept { ResourceTraits<T>::Unload(resource); }

	ResourceHandle(const ResourceHandle&) = delete;
	ResourceHandle& operator=(const ResourceHandle&) = delete;
	ResourceHandle(ResourceHandle&&) = delete;
	ResourceHandle& operator=(ResourceHandle&&) = delete;

	[[nodiscard]] const T& Get() const noexcept { return resource; }
};