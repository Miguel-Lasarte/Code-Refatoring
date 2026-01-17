#pragma once
#include "raylib.h"
#include <string_view>
#include <stdexcept>

template<typename T>
struct ResourceTraits;

template<>
struct ResourceTraits<Texture2D> {
	static Texture2D Load(std::string_view filePath) {
		Texture2D texture = LoadTexture(filePath.data());
		if(texture.id == 0) {
			throw std::runtime_error("Failed to load texture: " + std::string(filePath));
		}
		return texture;

	}
	static void Unload(Texture2D resource) {
		UnloadTexture(resource);
	}
};

template<>
struct ResourceTraits<Sound> {
	static Sound Load(std::string_view filePath) {
		Sound sound = LoadSound(filePath.data());
		if(sound.frameCount == 0) {
			throw std::runtime_error("Failed to load sound: " + std::string(filePath));
		}
		return sound;
	}
	static void Unload(Sound resource) {
		UnloadSound(resource);
	}
};

template<typename T>
class ResourceHandle {
	T resource;

public:

	explicit ResourceHandle(std::string_view filePath) {
		resource = ResourceTraits<T>::Load(filePath);

	}

	~ResourceHandle() {
		ResourceTraits<T>::Unload(resource);
	}

	ResourceHandle(ResourceHandle&& other) noexcept
		: resource(other.resource) {
		other.resource = T{};
	}

	ResourceHandle& operator=(ResourceHandle&& other) noexcept {
		if (this != &other) {
			ResourceTraits<T>::Unload(resource);
			resource = other.resource;
			other.resource = T{};
		}
		return *this;
	}

	ResourceHandle(const ResourceHandle&) = delete;
	ResourceHandle& operator=(const ResourceHandle&) = delete;

	[[nodiscard]] const T& Get() const noexcept {
		return resource;
	}

	operator T&() const noexcept {
		return resource;
	}
};