#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>

class ImageLoader {
private:
  static std::unordered_map<std::string, Texture2D> cache;
  static bool initialized;

public:
  static void Initialize();
  static Texture2D *GetImage(const std::string &name, const std::string &path);
  static Texture2D *GetImageFromUrl(const std::string &url);

  static void Cleanup();
};
