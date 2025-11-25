#pragma once
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
  ar(cereal::make_nvp("Url", t.Url),
     cereal::make_nvp("NombreTraductor", t.NombreTraductor),
     cereal::make_nvp("UrlImagenes", t.UrlImagenes),
     cereal::make_nvp("UnfilteredUrl", t.UnfilterdUrl));
}
template <class Archive> void serialize(Archive &ar, Capitulo &c) {
  ar(cereal::make_nvp("Numero", c.NumCapitulo),
     cereal::make_nvp("Nombre", c.NameCapitulo),
     cereal::make_nvp("Traducciones", c.traducciones),
     cereal::make_nvp("CantidadTraducciones", c.NumTraduciones));
}

template <class Archive> void serialize(Archive &ar, Manga &m) {
  ar(cereal::make_nvp("Nombre", m.nombre),
     cereal::make_nvp("URlbase", m.baseurl),
     cereal::make_nvp("Capitulos", m.capitulos),
     cereal::make_nvp("CantidadCapitulos", m.numerocapitulos));
}

} // namespace cereal

inline void SavetoDB(const Manga &manga) {
  std::string filename = manga.nombre;
  std::string path = GetSaveDir() + "/" + filename + ".json";
  std::ofstream os(path, std::ios::binary);
  cereal::JSONOutputArchive archive(os); // out
  archive(cereal::make_nvp("manga", manga));
  archive(manga);
}

inline Manga LoadFromDB(const std::filesystem::path &filepath) {
  std::ifstream is(filepath, std::ios::binary);
  cereal::JSONInputArchive archive(is);
  Manga manga;
  archive(cereal::make_nvp("manga", manga));
  return manga;
}

inline std::vector<Manga> LoadAllMangas() {
  std::vector<Manga> list;
  std::filesystem::path dir = GetSaveDir();

  for (const auto &file : std::filesystem::directory_iterator(dir)) {
    if (file.is_regular_file() && file.path().extension() == ".json") {
      try {
        Manga m = LoadFromDB(file.path());
        list.push_back(std::move(m));
      } catch (...) {
        std::cerr << "Error cargando archivo: " << file.path() << "\n";
      }
    }
  }

  return list;
}
