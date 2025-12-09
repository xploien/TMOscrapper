#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>

class ImageLoader
{
private:
  static std::unordered_map<std::string, Texture2D> cache;
  static bool initialized;

public:
  // Inicializar el cargador (llamar después de InitWindow)
  static void Initialize();

  // Obtener imagen (carga solo la primera vez)
  static Texture2D *GetImage(const std::string &name, const std::string &path);

  // Limpiar cache
  static void Cleanup();
};
