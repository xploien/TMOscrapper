#include "ImageLoader.hpp"
#include "../Curl.hpp"
#include <curl/curl.h>
#include <iostream>
#include <raylib.h>
#define SIMPLEWEBP_IMPLEMENTATION // requiere estar antes del include porque si
                                  // no se enoja
#include "../include/simplewebp/simplewebp.h"

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

Texture2D *ImageLoader::GetImageFromUrl(const std::string &url) {

  if (!initialized) {
    std::cerr << "ImageLoader not initialized!" << std::endl;
    return nullptr;
  }
  // checar cache
  auto it = cache.find(url);
  if (it != cache.end()) {
    return &it->second;
  }

  std::string buffer;
  buffer.reserve(1024 * 1024);
  // sera que 1mb sera suficiente para resultados de manga? TODO

  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Curl Initialize error\n";
    return nullptr;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

  // odio curl
  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    std::cerr << "Error downloading: " << curl_easy_strerror(res) << "\n";
    return nullptr;
  }

  if (buffer.empty()) {
    std::cerr << "Empty buffer downloaded\n";
    return nullptr;
  }

  std::cout << "DEBUG: Downloaded " << buffer.size()
            << " bytes. Decoding with SimpleWebP...\n";

  simplewebp *swebp = nullptr;
  // mi vida es nullptr

  // Cargar WebP desde memoria (SimpleWebP necesita void*, no const void*)
  simplewebp_error err = simplewebp_load_from_memory(
      const_cast<void *>(static_cast<const void *>(buffer.data())),
      buffer.size(),
      NULL, // usar allocator por defecto
      &swebp);

  if (err != SIMPLEWEBP_NO_ERROR) {
    std::cerr << "FAILED: SimpleWebP could not load image: "
              << simplewebp_get_error_text(err) << "\n";
    return nullptr;
  }

  size_t width, height;
  simplewebp_get_dimensions(swebp, &width, &height);

  std::cout << "DEBUG: Decoded successfully. Size: " << width << "x" << height
            << "\n";

  // Alocar buffer para pixeles RGBA (4 bytes por px)
  unsigned char *decodedData = new unsigned char[width * height * 4];

  err = simplewebp_decode(swebp, decodedData, NULL);

  if (err != SIMPLEWEBP_NO_ERROR) {
    std::cerr << "FAILED: SimpleWebP could not decode image: "
              << simplewebp_get_error_text(err) << "\n";
    simplewebp_unload(swebp);
    delete[] decodedData;
    return nullptr;
  }

  // Liberar
  simplewebp_unload(swebp);

  // construir a mano
  Image img = {0};
  img.data = decodedData;
  img.width = static_cast<int>(width);
  img.height = static_cast<int>(height);
  img.mipmaps = 1;
  img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8; // RGBA

  // Subir imagen a GPU
  Texture2D finalimg = LoadTextureFromImage(img);

  // liberar2
  delete[] decodedData;

  // Validar (nonsense? si se decodifico mal seguira siendo un array valido (?)
  if (finalimg.id == 0) {
    std::cerr << "Failed to create texture from image data\n";
    return nullptr;
  }

  cache[url] = finalimg;
  return &cache[url];
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
