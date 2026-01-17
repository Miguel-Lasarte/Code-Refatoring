#pragma once
#include "raylib.h"

template<typename T>
struct ResourceTraits;

template<>
struct ResourceTraits<Texture2D> {
	static Texture2D Load(const char* filePath) {
		return LoadTexture(filePath);
	}
	static void Unload(Texture2D resource) {
		UnloadTexture(resource);
	}
	static Texture2D Default() {
		return Texture2D{};
	}
};

template<>
struct ResourceTraits<Sound> {
	static Sound Load(const char* filePath) {
		return LoadSound(filePath);
	}
	static void Unload(Sound resource) {
		UnloadSound(resource);
	}
	static Sound Default() {
		return Sound{};
	}
};

template<typename T>
class ResourceHandle {
	T resource;

public:

	ResourceHandle(const char* filePath) {
		resource = ResourceTraits<T>::Load(filePath);

	}

	~ResourceHandle() {
		ResourceTraits<T>::Unload(resource);
	}


	[[nodiscard]] T Get() const noexcept {
		return resource;
	}

	operator T() const noexcept {
		return resource;
	}
};