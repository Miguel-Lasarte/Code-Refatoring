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
    static bool IsValid(const Texture2D& resource) {
        return resource.id != 0;
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
    static bool IsValid(const Sound& resource) {
		return resource.frameCount != 0;
    }
};

template<typename T>
class ResourceHandle {
    T resource;
    bool isLoaded = false;

public:
    ResourceHandle() = default;

    explicit ResourceHandle(const char* filePath) {
        resource = ResourceTraits<T>::Load(filePath);
        isLoaded = ResourceTraits<T>::IsValid(resource);
        if(!isLoaded) {
            resource = ResourceTraits<T>::Default();
		}
    }

    ~ResourceHandle() {
        if (isLoaded) {
            ResourceTraits<T>::Unload(resource);
        }
    }

    ResourceHandle(const ResourceHandle&) = delete;
    ResourceHandle& operator=(const ResourceHandle&) = delete;

    ResourceHandle(ResourceHandle&& other) noexcept
        : resource(other.resource), isLoaded(other.isLoaded) {
        other.isLoaded = false;
        other.resource = ResourceTraits<T>::Default();
    }

    ResourceHandle& operator=(ResourceHandle&& other) noexcept {
        if (this != &other) {
            if (isLoaded) {
                ResourceTraits<T>::Unload(resource);
            }
            resource = other.resource;
            isLoaded = other.isLoaded;
            other.isLoaded = false;
            other.resource = ResourceTraits<T>::Default();
        }
        return *this;
    }

    [[nodiscard]] T Get() const noexcept {
        return resource;
    }

    [[nodiscard]] bool IsLoaded() const noexcept {
        return isLoaded;
    }

    operator T() const noexcept {
        return resource;
    }
};