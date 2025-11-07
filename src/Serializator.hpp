#pragma once

#include "MangaData.hpp"
#include "include/cereal/archives/json.hpp"
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>

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

inline void SavetoDB(const Manga &manga, const std::string &filename) {
  std::ofstream os(filename, std::ios::binary);
  cereal::JSONOutputArchive archive(os); // out
  archive(manga);
}

inline Manga LoadFromDB(const std::string &filename) {
  std::ifstream is(filename, std::ios::binary);
  cereal::JSONInputArchive archive(is); // in
  Manga manga;
  archive(manga);
  return manga;
}
