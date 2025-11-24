#pragma once
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#define NO
#include "MangaData.hpp"
#include "include/cereal/archives/json.hpp"
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>

// ---------------------------------------------------------
//  Obtener ruta REAL del ejecutable (resuelve symlinks).
//  Luego construir <exeDir>/savedMangas
// ---------------------------------------------------------
inline std::string GetSaveDir() {
#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else // Linux
#include <unistd.h>
#include <vector>
#endif

  std::filesystem::path exePath;

  // ---------------------------
  // WINDOWS
  // ---------------------------
#if defined(_WIN32)
  char buffer[MAX_PATH];
  DWORD size = GetModuleFileNameA(NULL, buffer, MAX_PATH);
  exePath = std::filesystem::path(buffer);

  // ---------------------------
  // MACOS
  // ---------------------------
#elif defined(__APPLE__)
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size); // obtener tamaño necesario

  std::vector<char> buffer(size);
  _NSGetExecutablePath(buffer.data(), &size);

  exePath = std::filesystem::canonical(buffer.data());

  // ---------------------------
  // LINUX / UNIX
  // ---------------------------
#else
  std::vector<char> buffer(4096);
  ssize_t len = readlink("/proc/self/exe", buffer.data(), buffer.size());
  if (len > 0)
    exePath = std::filesystem::canonical(std::string(buffer.data(), len));
#endif

  // obtener directorio real
  std::filesystem::path exeDir = exePath.parent_path();

  // crear "savedMangas"
  std::filesystem::path saveDir = exeDir / "savedMangas";

  if (!std::filesystem::exists(saveDir)) {
    std::filesystem::create_directories(saveDir);
  }

  return saveDir.string();
}

namespace cereal {
template <class Archive> void serialize(Archive &ar, Traducion &t) {
  ar(t.Url, t.NombreTraductor, t.UrlImagenes, t.UnfilterdUrl);
}

template <class Archive> void serialize(Archive &ar, Capitulo &c) {
  ar(c.NumCapitulo, c.NameCapitulo, c.traducciones, c.NumTraduciones);
}

template <class Archive> void serialize(Archive &ar, Manga &m) {
  ar(m.nombre, m.capitulos, m.numerocapitulos);
}
} // namespace cereal

inline void SavetoDB(const Manga &manga) {
  std::string filename = manga.nombre;
  std::string path = GetSaveDir() + filename;
  std::ofstream os(path, std::ios::binary);
  cereal::JSONOutputArchive archive(os); // out
  archive(manga);
}

inline Manga LoadFromDB(const Manga &selmanga) {
  std::string filename = selmanga.nombre;
  std::string path = GetSaveDir() + filename;
  std::ifstream is(path, std::ios::binary);
  cereal::JSONInputArchive archive(is); // in
  Manga manga;
  archive(manga);
  return manga;
}
