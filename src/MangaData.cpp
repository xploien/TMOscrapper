#include "MangaData.hpp"
#include "MyApi.hpp"
#include "Tools.hpp"
#include <iostream>

std::string MangaData::FindMangaURl(std::string titulo) {
  MyApi Api;
  std::string pagina =
      Api.GetRawPage(titulo, "https://zonatmo.com/library?title=");

  std::vector<std::string> Entradas =
      Api.filterPage(pagina, "zonatmo.com/library/");

  std::cout << "Ingresa el numero del resultado a usar: \n";
  for (size_t h = 0; h < Entradas.size(); ++h) {
    std::cout << "opcion " << h << ": " << Entradas[h] << "\n";
  }
  int selecionado;
  std::cin >> selecionado;

  return Entradas[selecionado];
}

Manga MangaData::GetMangaFromUrl(std::string Url) {
  MyApi Api;
  Manga MangaObtenido;
  std::string manga = Api.GetRawPage("", Url);
  std::cout << "la pagina a usar sera: " << Url << "\n";

  MangaObtenido.capitulos = Api.GetChapters(manga, "fa fa-chevron-down fa-fw");
  return MangaObtenido;
}
