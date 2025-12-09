#include "ImageLoader.hpp"
#include <iostream>

std::unordered_map<std::string, Texture2D> ImageLoader::cache;
bool ImageLoader::initialized = false;

void ImageLoader::Initialize() {
  if (!initialized) {
    std::cout << "ImageLoader initialized" << std::endl;
    initialized = true;
  }
}

Texture2D *ImageLoader::GetImage(const std::string &name,
                                 const std::string &path) {
  if (!initialized) {
    std::cerr << "ImageLoader not initialized!" << std::endl;
    return nullptr;
  }

  // Buscar en cache primero
  auto it = cache.find(name);
  if (it != cache.end()) {
    return &it->second; // Ya existe, regresar referencia
  }

  // No existe, cargar por primera vez
  std::cout << "Loading image: " << name << " from " << path << std::endl;
  Texture2D texture = LoadTexture(path.c_str());

  if (texture.id == 0) {
    std::cerr << "Failed to load: " << path << std::endl;
    return nullptr;
  }

  // Guardar en cache y regresar referencia
  cache[name] = texture;
  return &cache[name];
}

void ImageLoader::Cleanup() {
  std::cout << "Cleaning up ImageLoader..." << std::endl;
  for (auto &[name, texture] : cache) {
    if (texture.id != 0) {
      UnloadTexture(texture);
      std::cout << "Unloaded: " << name << std::endl;
    }
  }
  cache.clear();
  initialized = false;
}
